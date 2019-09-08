#include <cassert>
#include "DownloadMode.h"
#include "GitCommand.h"
#include "Settings.h"
#include "Configuration.h"
#include "Helpers.h"

namespace {

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

const char *librariesArtifactsBranch()
{
	// TODO: Add Emscripten and Android branches

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

const char *engineArtifactsBranch()
{
	// TODO: Add Emscripten branch

	static std::string branchName;
	branchName = "master";
	config().branchName(branchName);

	branchName = "nCine-" + branchName;
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
	return branchName.data();
}

void downloadLibrariesArtifact(GitCommand &git)
{
	if (config().withMinGW())
	{
		Helpers::error("MinGW libraries artifacts do not exist");
		Helpers::info("Use pacman to download and install the required dependencies");
		return;
	}

	git.clone(Helpers::nCineLibrariesArtifactsRepositoryUrl(), librariesArtifactsBranch(), 1, true);
	git.checkout(Helpers::nCineLibrariesArtifactsSourceDir(), librariesArtifactsBranch(), nullptr);
}

void downloadLibraries(GitCommand &git)
{
	git.clone(Helpers::nCineLibrariesRepositoryUrl());
}

void downloadEngineArtifact(GitCommand &git)
{
	git.clone(Helpers::nCineArtifactsRepositoryUrl(), engineArtifactsBranch(), 1, true);
	git.checkout(Helpers::nCineArtifactsSourceDir(), engineArtifactsBranch(), nullptr);
}

void downloadEngine(GitCommand &git)
{
	git.clone(Helpers::nCineDataRepositoryUrl());
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

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void DownloadMode::perform(GitCommand &git, const Settings &settings)
{
	assert(settings.mode() == Settings::Mode::DOWNLOAD);

	switch (settings.downloadMode())
	{
		case Settings::DownloadMode::LIBS:
			if (settings.downloadArtifact())
				downloadLibrariesArtifact(git);
			else
				downloadLibraries(git);
			break;
		case Settings::DownloadMode::ENGINE:
			if (settings.downloadArtifact())
				downloadEngineArtifact(git);
			else
				downloadEngine(git);
			break;
		default:
			break;
	}
}
