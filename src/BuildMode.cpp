#include <cassert>
#include "BuildMode.h"
#include "CMakeCommand.h"
#include "FileSystem.h"
#include "Settings.h"
#include "Configuration.h"
#include "Helpers.h"

namespace {

const char *settingsToBuildConfigString(Settings::BuildType buildType)
{
	switch (buildType)
	{
		case Settings::BuildType::DEBUG: return "debug";
		case Settings::BuildType::RELEASE: return "release";
	}
	return nullptr;
}

void buildLibraries(CMakeCommand &cmake, const Settings &settings)
{
	Helpers::info("Build the libraries");

	std::string buildDir = Helpers::nCineLibrariesSourceDir();
	Helpers::buildDir(buildDir);

	bool hasBuilt = false;
	if (CMakeCommand::generatorIsMultiConfig())
		cmake.buildConfig(buildDir.data(), settingsToBuildConfigString(settings.buildType()));
	else
		hasBuilt = cmake.build(buildDir.data());

#if !defined(_WIN32) && !defined(__APPLE__)
	if (hasBuilt) // Overwrite `CMAKE_PREFIX_PATH` variable in any case
	{
		std::string absolutePath = fs::currentDir();
		absolutePath = fs::joinPath(absolutePath, Helpers::nCineExternalDir());

		if (fs::isDirectory(absolutePath.data()))
		{
			config().setCMakePrefixPath(absolutePath);
			config().save();
			Helpers::info("Set 'CMAKE_PREFIX_PATH' CMake variable to: ", absolutePath.data());
		}
	}
#endif
}

void buildAndroidLibraries(CMakeCommand &cmake, const Settings &settings)
{
	cmake.addNMakeDirToPath();

	Helpers::info("Build the Android libraries");

	std::string buildDir = Helpers::nCineAndroidLibrariesSourceDir();
	Helpers::buildDir(buildDir);

	cmake.build(buildDir.data());
}

void buildEngine(CMakeCommand &cmake, const Settings &settings)
{
	if (config().platform() == Configuration::Platform::ANDROID)
		cmake.addNMakeDirToPath();

	Helpers::info("Build the engine");

	std::string buildDir = Helpers::nCineSourceDir();
	Helpers::buildDir(buildDir);

	bool hasBuilt = false;
	if (CMakeCommand::generatorIsMultiConfig())
		hasBuilt = cmake.buildConfig(buildDir.data(), settingsToBuildConfigString(settings.buildType()));
	else
		hasBuilt = cmake.build(buildDir.data());

	if (hasBuilt) // Overwrite `nCine_DIR` variable in any case
	{
		std::string absolutePath = fs::currentDir();
		absolutePath = fs::joinPath(absolutePath, buildDir);
		if (fs::isDirectory(absolutePath.data()))
		{
			config().setEngineDir(absolutePath);
			config().save();
			Helpers::info("Set 'nCine_DIR' CMake variable to: ", absolutePath.data());
		}
	}
}

void buildGame(CMakeCommand &cmake, const Settings &settings, const std::string &gameName)
{
	if (config().platform() == Configuration::Platform::ANDROID)
	{
		cmake.addNMakeDirToPath();
		// Needed by Gradle to strip libraries
		cmake.addAndroidNdkDirToPath();
		cmake.addAndroidSdkDirToPath();
	}

	Helpers::info("Build the game: ", gameName.data());

	std::string buildDir = gameName;
	Helpers::buildDir(buildDir);

	if (CMakeCommand::generatorIsMultiConfig())
		cmake.buildConfig(buildDir.data(), settingsToBuildConfigString(settings.buildType()));
	else
		cmake.build(buildDir.data());
}

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void BuildMode::perform(CMakeCommand &cmake, const Settings &settings)
{
	assert(settings.mode() == Settings::Mode::BUILD);

	switch (settings.target())
	{
		case Settings::Target::LIBS:
			if (config().platform() == Configuration::Platform::ANDROID)
				buildAndroidLibraries(cmake, settings);
			else
				buildLibraries(cmake, settings);
			break;
		case Settings::Target::ENGINE:
			buildEngine(cmake, settings);
			break;
		case Settings::Target::GAME:
		{
			std::string gameName;
			config().gameName(gameName);

			buildGame(cmake, settings, gameName);
			break;
		}
	}
}
