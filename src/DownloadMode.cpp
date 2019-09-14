#include <cassert>
#include "DownloadMode.h"
#include "GitCommand.h"
#include "Settings.h"
#include "Configuration.h"
#include "Helpers.h"

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
	if (config().platform() == Configuration::Platform::ANDROID)
		git.clone(Helpers::nCineAndroidLibrariesRepositoryUrl());
	else
		git.clone(Helpers::nCineLibrariesRepositoryUrl());
}

void downloadEngineArtifact(GitCommand &git)
{
	git.clone(Helpers::nCineArtifactsRepositoryUrl(), artifactsBranch("nCine"), 1, true);
	git.checkout(Helpers::nCineArtifactsSourceDir(), artifactsBranch("nCine"), nullptr);
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

void downloadGameArtifact(GitCommand &git, const std::string &gameName)
{
	assert(gameName.empty() == false);

	git.clone(Helpers::gameArtifactsRepositoryUrl(gameName).data(), artifactsBranch(gameName.data()), 1, true);
	git.checkout(Helpers::gameArtifactsSourceDir(gameName).data(), artifactsBranch(gameName.data()), nullptr);
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

void DownloadMode::perform(GitCommand &git, const Settings &settings)
{
	assert(settings.mode() == Settings::Mode::DOWNLOAD);

	switch (settings.target())
	{
		case Settings::Target::LIBS:
			if (settings.downloadArtifact())
				downloadLibrariesArtifact(git);
			else
				downloadLibraries(git);
			break;
		case Settings::Target::ENGINE:
			if (settings.downloadArtifact())
				downloadEngineArtifact(git);
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
				downloadGameArtifact(git, gameName);
			else
				downloadGame(git, gameName);
			break;
		}
	}
}
