#include <cassert>
#include <iostream>
#include "ConfMode.h"
#include "CMakeCommand.h"
#include "Settings.h"
#include "Helpers.h"

namespace {

const char *settingsToBuildTypeString(Settings::BuildType buildType)
{
	switch (buildType)
	{
		case Settings::BuildType::DEBUG: return "Debug";
		case Settings::BuildType::RELEASE: return "Release";
	}
	return nullptr;
}

void configureLibraries(CMakeCommand &cmake, const Settings &settings)
{
	Helpers::info("Configure the libraries");

	std::string buildDir = Helpers::nCineLibrariesSourceDir();
	Helpers::buildDir(buildDir, settings);

	if (CMakeCommand::generatorIsMultiConfig())
		cmake.configure(Helpers::nCineLibrariesSourceDir(), buildDir.data());
	else
	{
		std::string arguments = "-D CMAKE_BUILD_TYPE=";
		arguments += settingsToBuildTypeString(settings.buildType());
		cmake.configure(Helpers::nCineLibrariesSourceDir(), buildDir.data(), arguments.data());
	}
}

void configureEngine(CMakeCommand &cmake, const Settings &settings)
{
	Helpers::info("Configure the engine");

	std::string buildDir = Helpers::nCineSourceDir();
	Helpers::buildDir(buildDir, settings);

	if (CMakeCommand::generatorIsMultiConfig())
		cmake.configure(Helpers::nCineSourceDir(), buildDir.data());
	else
	{
		std::string arguments = "-D CMAKE_BUILD_TYPE=";
		arguments += settingsToBuildTypeString(settings.buildType());
		cmake.configure(Helpers::nCineSourceDir(), buildDir.data(), arguments.data());
	}
}

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ConfMode::perform(CMakeCommand &cmake, const Settings &settings)
{
	assert(settings.mode() == Settings::Mode::CONF);

	switch (settings.confMode())
	{
		case Settings::ConfMode::LIBS: configureLibraries(cmake, settings); break;
		case Settings::ConfMode::ENGINE: configureEngine(cmake, settings); break;
		default: break;
	}
}
