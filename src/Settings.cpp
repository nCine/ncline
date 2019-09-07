#include "Settings.h"
#include "Configuration.h"
#include "CMakeCommand.h"
#include "version.h"
#include <iostream>
#include <clipp.h>

using namespace clipp;

namespace {
const char *programName = "ncline";
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool Settings::parseArguments(int argc, char **argv)
{
	// clang-format off
	auto setMode = (command("set").set(mode_, Mode::SET),
	                (
	                    option("-ninja").call([] { config().setWithNinja(true); }) |
	                    option("-no-ninja").call([] { config().setWithNinja(false); })
	                ).doc("(do not) prefer Ninja as a CMake generator"),
#ifdef _WIN32
	                (
	                    option("-mingw").call([] { config().setWithMinGW(true); }) |
	                    option("-no-mingw").call([] { config().setWithMinGW(false); })
	                ).doc("(do not) use MinGW/MSYS environment to configure and build"),
	                (
	                    option("-vs2017").call([] { config().setVsVersion(2017); }) |
	                    option("-vs2019").call([] { config().setVsVersion(2019); })
	                ).doc("(do not) use MinGW/MSYS environment to configure and build"),
#endif
	                (
	                    option("-colors").call([] { config().setWithColors(true); }) |
	                    option("-no-colors").call([] { config().setWithColors(false); })
	                ).doc("(do not) use shell colors in the output")
	               );

	auto downloadMode = (command("download").set(mode_, Mode::DOWNLOAD).doc("download mode"),
	                     command("libs").set(downloadMode_, DownloadMode::LIBS) |
	                     command("engine").set(downloadMode_, DownloadMode::ENGINE) |
	                     command("game").set(downloadMode_, DownloadMode::GAME));

	auto confMode = (command("conf").set(mode_, Mode::CONF).doc("configuration mode"),
	                 command("libs").set(confMode_, ConfMode::LIBS) |
	                 command("engine").set(confMode_, ConfMode::ENGINE) |
	                 command("game").set(confMode_, ConfMode::GAME));

	auto buildMode = (command("build").set(mode_, Mode::BUILD).doc("build mode"),
	                  command("libs").set(buildMode_, BuildMode::LIBS) |
	                  command("engine").set(buildMode_, BuildMode::ENGINE) |
	                  command("game").set(buildMode_, BuildMode::GAME));

	auto &buildTypeMode = CMakeCommand::generatorIsMultiConfig() ? buildMode : confMode;
	buildTypeMode.push_back(command("debug").set(buildType_, BuildType::DEBUG) | command("release").set(buildType_, BuildType::RELEASE).doc("choose debug or release build type"));

	auto cli = ((setMode | downloadMode | confMode | buildMode |
	             command("help").set(mode_, Mode::HELP).doc("show help") |
	             command("--version").set(mode_, Mode::VERSION).doc("show version")));
	// clang-format on

	bool parsed = false;
	if (parse(argc, argv, cli))
	{
		parsed = true;
		if (mode_ == Mode::HELP)
			std::cout << make_man_page(cli, programName);
		else if (mode_ == Mode::VERSION)
			std::cout << programName << " version " << VersionStrings::Version << "\n";
	}
	else
		std::cout << usage_lines(cli, programName) << '\n';

	return parsed;
}
