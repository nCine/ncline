#include <cassert>
#include <iostream>
#include "ConfMode.h"
#include "CMakeCommand.h"
#include "Settings.h"

namespace {

const char *nCineLibrariesSourceDir = "nCine-libraries";
const char *nCineSourceDir = "nCine";

void configureLibraries(CMakeCommand &cmake, const Settings &settings)
{
	std::cout << "-> Configure libraries\n";

	std::string binaryDir = nCineLibrariesSourceDir;
	binaryDir += (settings.buildType() == Settings::BuildType::DEBUG) ? "-debug" : "-release";

	std::string arguments = "-D CMAKE_BUILD_TYPE=";
	arguments += (settings.buildType() == Settings::BuildType::DEBUG) ? "Debug" : "Release";
	cmake.configure(nCineLibrariesSourceDir, binaryDir.data(), arguments.data());
}

void configureEngine(CMakeCommand &cmake, const Settings &settings)
{
	std::cout << "-> Configure the engine\n";

	std::string binaryDir = nCineSourceDir;
	binaryDir += (settings.buildType() == Settings::BuildType::DEBUG) ? "-debug" : "-release";

	std::string arguments = "-D CMAKE_BUILD_TYPE=";
	arguments += (settings.buildType() == Settings::BuildType::DEBUG) ? "Debug" : "Release";
	cmake.configure(nCineSourceDir, binaryDir.data(), arguments.data());
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
