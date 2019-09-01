#include <cassert>
#include <iostream>
#include "BuildMode.h"
#include "CMakeCommand.h"
#include "Settings.h"

namespace {

const char *nCineLibrariesSourceDir = "nCine-libraries";
const char *nCineSourceDir = "nCine";

void buildLibraries(CMakeCommand &cmake, const Settings &settings)
{
	std::cout << "-> Build the libraries\n";

	std::string buildDir = nCineLibrariesSourceDir;
	buildDir += (settings.buildType() == Settings::BuildType::DEBUG) ? "-debug" : "-release";
	cmake.build(buildDir.data());
}

void buildEngine(CMakeCommand &cmake, const Settings &settings)
{
	std::cout << "-> Build the engine\n";

	std::string buildDir = nCineSourceDir;
	buildDir += (settings.buildType() == Settings::BuildType::DEBUG) ? "-debug" : "-release";
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
