#include <cassert>
#include <iostream>
#include "BuildMode.h"
#include "CMakeCommand.h"
#include "Settings.h"
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
	Helpers::buildDir(buildDir, settings);

	if (CMakeCommand::generatorIsMultiConfig())
		cmake.buildConfig(buildDir.data(), settingsToBuildConfigString(settings.buildType()));
	else
		cmake.build(buildDir.data());
}

void buildEngine(CMakeCommand &cmake, const Settings &settings)
{
	Helpers::info("Build the engine");

	std::string buildDir = Helpers::nCineSourceDir();
	Helpers::buildDir(buildDir, settings);

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

	switch (settings.buildMode())
	{
		case Settings::BuildMode::LIBS: buildLibraries(cmake, settings); break;
		case Settings::BuildMode::ENGINE: buildEngine(cmake, settings); break;
		default: break;
	}
}
