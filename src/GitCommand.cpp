#include <cassert>
#include <cstring>
#include <ctime>
#include <algorithm>
#include "GitCommand.h"
#include "Process.h"
#include "FileSystem.h"
#include "Configuration.h"
#include "Helpers.h"

using fs = FileSystem;

namespace {

const int MaxLength = 1024;
char buffer[MaxLength];

char *strncpyWrapper(char *dest, size_t elements, const char *source, size_t count)
{
#if defined(_WIN32) && !defined(__MINGW32__)
	strncpy_s(dest, elements, source, count);
	return dest;
#else
	return strncpy(dest, source, count);
#endif
}

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GitCommand::GitCommand()
    : found_(false)
{
	output_.reserve(1024);

	if (config().gitExecutable(executable_) == false)
	{
		if (checkPredefinedLocations() == false)
			executable_ = "git";
	}

	// Allow CMake ExternalProject to find git if it's not already in the path
	std::string gitDir = fs::dirName(executable_.data());
	if (gitDir.empty() == false && gitDir != ".")
	{
		gitDir = fs::absolutePath(gitDir.data());
		Helpers::info("Add Git directory to path: ", gitDir.data());
		Helpers::addDirToPath(gitDir.data());
	}

	if (executable_.find(' ') != std::string::npos)
		executable_ = "\"" + executable_ + "\"";

	snprintf(buffer, MaxLength, "%s --version", executable_.data());
	const bool executed = Process::executeCommand(buffer, output_, Process::Echo::DISABLED);
	if (executed)
		found_ = sscanf(output_.data(), "git version %u.%u.%u", &version_[0], &version_[1], &version_[2]);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool GitCommand::clone(const char *repositoryUrl, const char *branch, unsigned int depth, bool noCheckout)
{
	assert(found_);
	assert(repositoryUrl);
	assert(branch);

	std::string cloneCommand = executable() + " clone " + repositoryUrl;
	cloneCommand += " --single-branch --branch ";
	cloneCommand += branch;
	if (depth > 0)
		cloneCommand += " --depth " + std::to_string(depth);
	if (noCheckout)
		cloneCommand += " --no-checkout";

	const bool executed = Process::executeCommand(cloneCommand.data(), output_);
	return executed;
}

bool GitCommand::clone(const char *repositoryUrl)
{
	assert(found_);
	assert(repositoryUrl);

	snprintf(buffer, MaxLength, "%s clone %s", executable_.data(), repositoryUrl);
	const bool executed = Process::executeCommand(buffer, output_);
	return executed;
}

bool GitCommand::checkout(const char *repositoryDir, const char *branch, const char *workTreeDir)
{
	assert(found_);
	assert(repositoryDir);
	assert(branch);

	const std::string repositoryGitDir = fs::joinPath(repositoryDir, ".git");

	if (workTreeDir)
		snprintf(buffer, MaxLength, "%s --git-dir=%s --work-tree=%s checkout %s", executable_.data(), repositoryGitDir.data(), workTreeDir, branch);
	else
		snprintf(buffer, MaxLength, "%s --git-dir=%s checkout %s", executable_.data(), repositoryGitDir.data(), branch);
	const bool executed = Process::executeCommand(buffer, output_);
	return executed;
}

bool GitCommand::checkRepositoryVersion(const char *repositoryDir, std::string &version)
{
	const std::string repositoryGitDir = fs::joinPath(repositoryDir, ".git");

	if (found_ && fs::isDirectory(repositoryGitDir.data()))
	{
		bool executed = false;

		const unsigned int MaxLength = 128;
		char revCount[MaxLength];
		char shortHash[MaxLength];
		char lastCommitDate[MaxLength];
		char branchName[MaxLength];
		bool hasTag = false;
		char tagName[MaxLength];

		snprintf(buffer, MaxLength, "%s --git-dir=%s rev-list --count HEAD", executable_.data(), repositoryGitDir.data());
		executed = Process::executeCommand(buffer, output_, Process::Echo::DISABLED);
		assert(executed);
		strncpyWrapper(revCount, MaxLength, output_.data(), MaxLength - 1);

		snprintf(buffer, MaxLength, "%s --git-dir=%s rev-parse --short HEAD", executable_.data(), repositoryGitDir.data());
		executed = Process::executeCommand(buffer, output_, Process::Echo::DISABLED);
		assert(executed);
		strncpyWrapper(shortHash, MaxLength, output_.data(), MaxLength - 1);

		snprintf(buffer, MaxLength, "%s --git-dir=%s log -1 --format=%%ad --date=format:%%Y.%%m", executable_.data(), repositoryGitDir.data());
		executed = Process::executeCommand(buffer, output_, Process::Echo::DISABLED);
		assert(executed);
		strncpyWrapper(lastCommitDate, MaxLength, output_.data(), MaxLength - 1);

		snprintf(buffer, MaxLength, "%s --git-dir=%s rev-parse --abbrev-ref HEAD", executable_.data(), repositoryGitDir.data());
		executed = Process::executeCommand(buffer, output_, Process::Echo::DISABLED);
		assert(executed);
		strncpyWrapper(branchName, MaxLength, output_.data(), MaxLength - 1);

		snprintf(buffer, MaxLength, "%s --git-dir=%s describe --tags --exact-match HEAD", executable_.data(), repositoryGitDir.data());
		hasTag = Process::executeCommand(buffer, output_, Process::Echo::DISABLED);

		if (hasTag)
		{
			strncpyWrapper(tagName, MaxLength, output_.data(), MaxLength - 1);
			version = tagName;
		}
		else
			version = std::string(lastCommitDate) + ".r" + std::string(revCount) + "-" + std::string(shortHash);

		version.erase(std::remove(version.begin(), version.end(), '\n'), version.end());
	}
	else
	{
		time_t now = time(nullptr);
		struct tm tstruct;
		char timeString[80];
#ifdef _WIN32
		localtime_s(&tstruct, &now);
#else
		tstruct = *localtime(&now);
#endif
		strftime(timeString, sizeof(timeString), "%Y.%m.%d", &tstruct);
		version = timeString;
	}

	return true;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool GitCommand::checkPredefinedLocations()
{
	bool isAccessible = false;

#if defined(_WIN32)
	std::string programsToGit = "Git/bin/git.exe";

	if (fs::canAccess(fs::joinPath(Helpers::getEnvironment("ProgramW6432"), programsToGit).data()))
	{
		executable_ = fs::joinPath(Helpers::getEnvironment("ProgramW6432"), programsToGit);
		isAccessible = true;
	}
	else if (fs::canAccess(fs::joinPath(Helpers::getEnvironment("ProgramFiles"), programsToGit).data()))
	{
		executable_ = fs::joinPath(Helpers::getEnvironment("ProgramFiles"), programsToGit);
		isAccessible = true;
	}
#endif

	return isAccessible;
}
