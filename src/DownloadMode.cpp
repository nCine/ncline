#include <cassert>
#include <algorithm>
#include "DownloadMode.h"
#include "GitCommand.h"
#include "CMakeCommand.h"
#include "FileSystem.h"
#include "Settings.h"
#include "Configuration.h"
#include "Helpers.h"

#ifdef __APPLE__
	#include "Process.h"
#endif

using fs = FileSystem;

namespace {

bool gameNameIsCustom(const std::string &gameName)
{
	return (gameName != "ncTemplate" &&
	        gameName != "ncPong" &&
	        gameName != "ncInvaders" &&
	        gameName != "ncParticleEditor");
}

void appendCompilerString(std::string &branchName)
{
	switch (config().compiler())
	{
		case Configuration::Compiler::GCC:
		case Configuration::Compiler::UNSPECIFIED:
			branchName += "-gcc";
			break;
		case Configuration::Compiler::CLANG:
			branchName += "-clang";
			break;
	}
}

void appendAndroidArchString(std::string &branchName)
{
	switch (config().androidArch())
	{
		case Configuration::AndroidArch::ARMEABI_V7A:
			branchName += "-armeabi-v7a";
			break;
		case Configuration::AndroidArch::ARM64_V8A:
		case Configuration::AndroidArch::UNSPECIFIED:
			branchName += "-arm64-v8a";
			break;
		case Configuration::AndroidArch::X86_64:
			branchName += "-x86_64";
			break;
	}
}

const char *librariesArtifactsBranch()
{
	if (config().platform() == Configuration::Platform::ANDROID)
	{
		switch (config().androidArch())
		{
			case Configuration::AndroidArch::ARMEABI_V7A:
				return "android-libraries-armeabi-v7a";
			case Configuration::AndroidArch::ARM64_V8A:
			case Configuration::AndroidArch::UNSPECIFIED:
				return "android-libraries-arm64-v8a";
			case Configuration::AndroidArch::X86_64:
				return "android-libraries-x86_64";
		}
	}

	if (config().platform() == Configuration::Platform::EMSCRIPTEN)
		return "libraries-emscripten-emcc";

#if defined(__APPLE__)
	return "libraries-darwin-appleclang";
#elif defined(_WIN32)
	if (config().vsVersion() == 2019)
		return "libraries-windows-vs2019";
	else if (config().vsVersion() == 2017)
		return "libraries-windows-vs2019";
#else
	switch (config().compiler())
	{
		case Configuration::Compiler::GCC:
		case Configuration::Compiler::UNSPECIFIED:
			return "libraries-linux-gcc";
		case Configuration::Compiler::CLANG:
			return "libraries-linux-clang";
	}
#endif
	return nullptr;
}

const char *artifactsBranch(const char *project)
{
	assert(project);
	static std::string branchName;

	std::string configBranchName = "master";
	config().branchName(configBranchName);

	branchName = project;
	const bool isEngine = (branchName == "nCine");
	branchName += "-" + configBranchName;

	if (config().platform() == Configuration::Platform::ANDROID && isEngine == false)
	{
		branchName += "-android";
		appendAndroidArchString(branchName);
		branchName += "-Debug";
	}
	else if (config().platform() == Configuration::Platform::EMSCRIPTEN)
		branchName += "-emscripten-emcc";
	else
	{
#if defined(__APPLE__)
		branchName += "-darwin-appleclang";
#elif defined(_WIN32)
		if (config().withMinGW())
		{
			branchName += "-mingw64";
			appendCompilerString(branchName);
		}
		else
		{
			branchName += "-windows";
			if (config().vsVersion() == 2019)
				branchName += "-vs2019";
			else if (config().vsVersion() == 2017)
				branchName += "-vs2017";
		}
#else
		branchName += "-linux";
		appendCompilerString(branchName);
#endif
	}

	return branchName.data();
}

bool extractArchiveAndDeleteDir(CMakeCommand &cmake, const char *archiveFile, const char *directory)
{
	assert(archiveFile);
	assert(directory);

	std::string toolCommand = "remove_directory ";
	toolCommand += directory;
	bool executed = cmake.toolsMode(toolCommand.data());

	if (executed)
	{
		toolCommand = "tar xz ";
		toolCommand += archiveFile;
		executed = cmake.toolsMode(toolCommand.data());
	}

	if (executed)
	{
		toolCommand = "remove ";
		toolCommand += archiveFile;
		cmake.toolsMode(toolCommand.data());
	}

	return executed;
}

void downloadLibrariesArtifact(GitCommand &git, CMakeCommand &cmake)
{
	if (config().withMinGW())
	{
		Helpers::error("MinGW libraries artifacts do not exist");
		Helpers::info("Use pacman to download and install the required dependencies");
		return;
	}

	git.clone(Helpers::nCineLibrariesArtifactsRepositoryUrl(), librariesArtifactsBranch(), 1, true);
	git.checkout(Helpers::nCineLibrariesArtifactsSourceDir(), librariesArtifactsBranch(), nullptr);

	git.customCommand(Helpers::nCineLibrariesArtifactsSourceDir(), "ls-tree -r --name-only HEAD");

	std::string archiveFile = git.output();
	archiveFile.erase(std::remove(archiveFile.begin(), archiveFile.end(), '\n'), archiveFile.end());

	const bool hasExtracted = extractArchiveAndDeleteDir(cmake, archiveFile.data(), Helpers::nCineLibrariesArtifactsSourceDir());

#if !defined(_WIN32) && !defined(__APPLE__)
	if (config().platform() == Configuration::Platform::DESKTOP &&
	    hasExtracted && config().hasCMakePrefixPath() == false)
	{
		std::string absolutePath = fs::currentDir();
		absolutePath = fs::joinPath(absolutePath, Helpers::nCineExternalDir());

		if (fs::isDirectory(absolutePath.data()))
		{
			config().setCMakePrefixPath(absolutePath.data());
			config().save();
			Helpers::info("Set 'CMAKE_PREFIX_PATH' CMake variable to: ", absolutePath.data());
		}
	}
#endif
}

void downloadLibraries(GitCommand &git)
{
	if (config().platform() == Configuration::Platform::ANDROID)
		git.clone(Helpers::nCineAndroidLibrariesRepositoryUrl());
	else
		git.clone(Helpers::nCineLibrariesRepositoryUrl());
}

void downloadEngineArtifact(GitCommand &git, CMakeCommand &cmake)
{
	git.clone(Helpers::nCineArtifactsRepositoryUrl(), artifactsBranch("nCine"), 1, true);
	git.checkout(Helpers::nCineArtifactsSourceDir(), artifactsBranch("nCine"), nullptr);

	git.customCommand(Helpers::nCineArtifactsSourceDir(), "ls-tree -r --name-only HEAD");

	std::string archiveFile = git.output();
#ifdef _WIN32
	if (config().withMinGW() == false)
	{
		// Multiple archives in the branch
		std::string archiveFiles = git.output();
		size_t firstChar = 0;
		size_t lastChar = 0;
		while (firstChar != std::string::npos)
		{
			firstChar = lastChar;
			lastChar = archiveFiles.find('\n', firstChar);
			archiveFile = archiveFiles.substr(firstChar, lastChar);
			lastChar++;
			if (archiveFile.find(".zip") != std::string::npos)
				break;
			else
			{
				std::string toolCommand = "remove ";
				toolCommand += archiveFile;
				cmake.toolsMode(toolCommand.data());
			}
		}
	}
#endif

	archiveFile.erase(std::remove(archiveFile.begin(), archiveFile.end(), '\n'), archiveFile.end());
#ifndef __APPLE__
	const bool hasExtracted = extractArchiveAndDeleteDir(cmake, archiveFile.data(), Helpers::nCineArtifactsSourceDir());
#else
	std::string command = "remove_directory ";
	command += Helpers::nCineArtifactsSourceDir();
	cmake.toolsMode(command.data());

	command = "hdiutil convert " + archiveFile + " -format UDTO -o nCine";
	bool executed = Process::executeCommand(command.data());

	command = "remove " + archiveFile;
	cmake.toolsMode(command.data());
	archiveFile = archiveFile.substr(0, archiveFile.find(".dmg"));

	if (executed)
	{
		executed = Process::executeCommand("hdiutil attach -readonly nCine.cdr", Process::Echo::COMMAND_ONLY);

		if (executed)
		{
			command = "copy_directory /Volumes/" + archiveFile + "/nCine.app nCine.app";
			executed = cmake.toolsMode(command.data());

			command = "hdiutil detach /Volumes/" + archiveFile;
			Process::executeCommand(command.data(), Process::Echo::COMMAND_ONLY);
		}

		cmake.toolsMode("remove nCine.cdr");
	}
	const bool hasExtracted = executed;
#endif

	if (hasExtracted && config().hasEngineDir() == false)
	{
		std::string absolutePath = fs::currentDir();
#ifdef _WIN32
		if (config().withMinGW() == false)
		{
			// Extracting directory name from archive file name
			archiveFile = archiveFile.substr(0, archiveFile.find(".zip"));
			absolutePath = fs::joinPath(absolutePath, archiveFile);
			absolutePath = fs::joinPath(absolutePath, "cmake");
		}
		else
		{
			// MinGW is just like Linux
			archiveFile = archiveFile.substr(0, archiveFile.find(".tar.gz"));
			absolutePath = fs::joinPath(absolutePath, archiveFile);
			if (config().platform() == Configuration::Platform::EMSCRIPTEN)
				absolutePath = fs::joinPath(absolutePath, "cmake");
			else
				absolutePath = fs::joinPath(absolutePath, "lib/cmake/nCine");
		}
#elif __APPLE__
		absolutePath = fs::joinPath(absolutePath, "nCine.app");
		absolutePath = fs::joinPath(absolutePath, "Contents/Resources/cmake");
#else
		// Extracting directory name from archive file name
		archiveFile = archiveFile.substr(0, archiveFile.find(".tar.gz"));
		absolutePath = fs::joinPath(absolutePath, archiveFile);
		if (config().platform() == Configuration::Platform::EMSCRIPTEN)
			absolutePath = fs::joinPath(absolutePath, "cmake");
		else
			absolutePath = fs::joinPath(absolutePath, "lib/cmake/nCine");
#endif

		if (fs::isDirectory(absolutePath.data()))
		{
			config().setEngineDir(absolutePath.data());
			config().save();
			Helpers::info("Set 'nCine_DIR' CMake variable to: ", absolutePath.data());
		}
	}
}

void downloadEngine(GitCommand &git)
{
	git.clone(Helpers::nCineDataRepositoryUrl(), "master", 1);
	git.clone(Helpers::nCineRepositoryUrl());

	std::string branchName;
	if (config().branchName(branchName))
	{
		Helpers::info("Check out engine branch: ", branchName.data());
		git.checkout(Helpers::nCineSourceDir(), branchName.data());
	}

	std::string version;
	if (git.checkRepositoryVersion(Helpers::nCineSourceDir(), version))
		Helpers::info("Repository at version: ", version.data());
}

void downloadGameArtifact(GitCommand &git, CMakeCommand &cmake, const std::string &gameName)
{
	assert(gameName.empty() == false);

	git.clone(Helpers::gameArtifactsRepositoryUrl(gameName).data(), artifactsBranch(gameName.data()), 1, true);
	git.checkout(Helpers::gameArtifactsSourceDir(gameName).data(), artifactsBranch(gameName.data()), nullptr);

	// Don' try to extract files from an APK
	if (config().platform() != Configuration::Platform::ANDROID)
	{
		git.customCommand(Helpers::gameArtifactsSourceDir(gameName).data(), "ls-tree -r --name-only HEAD");
		extractArchiveAndDeleteDir(cmake, git.output().data(), Helpers::gameArtifactsSourceDir(gameName).data());
	}
}

void downloadGame(GitCommand &git, const std::string &gameName)
{
	assert(gameName.empty() == false);

	git.clone(Helpers::gameDataRepositoryUrl(gameName).data(), "master", 1);
	git.clone(Helpers::gameRepositoryUrl(gameName).data());

	std::string branchName;
	if (config().branchName(branchName))
	{
		Helpers::info("Check out game branch: ", branchName.data());
		git.checkout(gameName.data(), branchName.data());
	}

	std::string version;
	if (git.checkRepositoryVersion(gameName.data(), version))
		Helpers::info("Repository at version: ", version.data());
}

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void DownloadMode::perform(GitCommand &git, CMakeCommand &cmake, const Settings &settings)
{
	assert(settings.mode() == Settings::Mode::DOWNLOAD);

	switch (settings.target())
	{
		case Settings::Target::LIBS:
			if (settings.downloadArtifact())
				downloadLibrariesArtifact(git, cmake);
			else
				downloadLibraries(git);
			break;
		case Settings::Target::ENGINE:
			if (settings.downloadArtifact())
				downloadEngineArtifact(git, cmake);
			else
				downloadEngine(git);
			break;
		case Settings::Target::GAME:
		{
			std::string gameName;
			config().gameName(gameName);
			if (gameNameIsCustom(gameName))
			{
				Helpers::error("No official nCine game project with the specified name");
				Helpers::info("Don't use the 'download' command with a custom project");
				return;
			}

			if (settings.downloadArtifact())
				downloadGameArtifact(git, cmake, gameName);
			else
				downloadGame(git, gameName);
			break;
		}
	}
}
