#include <cassert>
#include <cstring>
#include <ctime>
#include "GitCommand.h"
#include "Process.h"
#include "FileSystem.h"

using fs = FileSystem;

namespace {

const int MaxLength = 1024;
char buffer[MaxLength];

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GitCommand::GitCommand(const char *executable)
    : found_(false), executable_(executable)
{
	output_.reserve(1024);

	snprintf(buffer, MaxLength, "%s --version", executable_.data());
	const bool executed = Process::executeCommand(buffer, output_, Process::Echo::DISABLED);
	if (executed)
		found_ = sscanf(output_.data(), "git version %d.%d.%d", &version_[0], &version_[1], &version_[2]);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool GitCommand::checkRepositoryVersion(const char *repository, std::string &version)
{
	std::string repositoryGitDir = fs::joinPath(repository, ".git");

	if (found_ && fs::isDirectory(repositoryGitDir.data()))
	{
		std::string versionCommand;
		bool executed = false;

		char revCount[8];
		char shortHash[64];
		char lastCommitDate[32];
		char branchName[128];
		bool hasTag = false;
		char tagName[128];

		output_.clear();
		versionCommand = executable_ + " --git-dir=" + repositoryGitDir + " rev-list --count HEAD";
		executed = Process::executeCommand(versionCommand.data(), output_);
		assert(executed);
		sscanf(output_.data(), "%7s", revCount);

		output_.clear();
		versionCommand = executable_ + " --git-dir=" + repositoryGitDir + " rev-parse --short HEAD";
		executed = Process::executeCommand(versionCommand.data(), output_);
		assert(executed);
		sscanf(output_.data(), "%63s", shortHash);

		output_.clear();
		versionCommand = executable_ + " --git-dir=" + repositoryGitDir + " log -1 --format=%ad --date=format:%Y.%m";
		executed = Process::executeCommand(versionCommand.data(), output_);
		assert(executed);
		sscanf(output_.data(), "%31s", lastCommitDate);

		output_.clear();
		versionCommand = executable_ + " --git-dir=" + repositoryGitDir + " rev-parse --abbrev-ref HEAD";
		executed = Process::executeCommand(versionCommand.data(), output_);
		assert(executed);
		sscanf(output_.data(), "%127s", branchName);

		output_.clear();
		versionCommand = executable_ + " --git-dir=" + repositoryGitDir + " describe --tags --exact-match HEAD";
		hasTag = Process::executeCommand(versionCommand.data(), output_);

		if (hasTag)
		{
			sscanf(output_.data(), "%127s", tagName);
			version = tagName;
		}
		else
		{
			version = std::string(lastCommitDate) + ".r" + std::string(revCount) + "-" + std::string(shortHash);
		}
	}
	else
	{
		time_t now = time(nullptr);
		struct tm tstruct;
		char timeString[80];
		tstruct = *localtime(&now);
		strftime(timeString, sizeof(timeString), "%Y.%m.%d", &tstruct);
		version = timeString;
	}

	return true;
}

bool GitCommand::clone(const char *repository, const char *branch)
{
	assert(found_);
	assert(branch);
	output_.clear();

	snprintf(buffer, MaxLength, "%s clone --single-branch --branch %s %s", executable_.data(), branch, repository);
	const bool executed = Process::executeCommand(buffer, output_);
	return executed;
}

bool GitCommand::clone(const char *repository)
{
	assert(found_);
	output_.clear();

	snprintf(buffer, MaxLength, "%s clone %s", executable_.data(), repository);
	const bool executed = Process::executeCommand(buffer, output_);
	return executed;
}
