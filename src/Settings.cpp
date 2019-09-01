#include "Settings.h"
#include "Configuration.h"
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
	                ).doc("(do not) prefer Ninja as a CMake generator"));

	auto downloadMode = (command("download").set(mode_, Mode::DOWNLOAD).doc("download mode"),
	                     command("libs").set(downloadMode_, DownloadMode::LIBS) |
	                     command("engine").set(downloadMode_, DownloadMode::ENGINE) |
	                     command("game").set(downloadMode_, DownloadMode::GAME));

	auto confMode = (command("conf").set(mode_, Mode::CONF).doc("configuration mode"),
	                 command("libs").set(confMode_, ConfMode::LIBS) |
	                 command("engine").set(confMode_, ConfMode::ENGINE) |
	                 command("game").set(confMode_, ConfMode::GAME),
	                 (
	                     command("debug").set(buildType_, BuildType::DEBUG) |
	                     command("release").set(buildType_, BuildType::RELEASE)
	                 ).doc("configure a debug or a release build"));

	auto buildMode = (command("build").set(mode_, Mode::BUILD).doc("build mode"),
	                  command("libs").set(buildMode_, BuildMode::LIBS) |
	                  command("engine").set(buildMode_, BuildMode::ENGINE) |
	                  command("game").set(buildMode_, BuildMode::GAME));

	auto cli = ((setMode | downloadMode | confMode | buildMode | command("help").set(mode_, Mode::HELP)),
	            option("-v", "--version").call([] { std::cout << "version " << VersionStrings::Version << "\n\n"; }).doc("show version"));
	// clang-format on

	bool parsed = false;
	if (parse(argc, argv, cli))
	{
		parsed = true;
		if (mode_ == Mode::HELP)
			std::cout << make_man_page(cli, programName);
	}
	else
		std::cout << usage_lines(cli, programName) << '\n';

	return parsed;
}

void Settings::print()
{
	std::cout << "Mode ";
	switch (mode_)
	{
		case Mode::SET: std::cout << "SET"; break;
		case Mode::DOWNLOAD: std::cout << "DOWNLOAD"; break;
		case Mode::CONF: std::cout << "CONF"; break;
		case Mode::BUILD: std::cout << "BUILD"; break;
		case Mode::DIST: std::cout << "DIST"; break;
		case Mode::HELP: std::cout << "HELP"; break;
	}
	std::cout << '\n';

	std::cout << "DownloadMode ";
	switch (downloadMode_)
	{
		case DownloadMode::LIBS: std::cout << "LIBS"; break;
		case DownloadMode::ENGINE: std::cout << "ENGINE"; break;
		case DownloadMode::GAME: std::cout << "GAME"; break;
	}
	std::cout << '\n';

	std::cout << "ConfMode ";
	switch (confMode_)
	{
		case ConfMode::LIBS: std::cout << "LIBS"; break;
		case ConfMode::ENGINE: std::cout << "ENGINE"; break;
		case ConfMode::GAME: std::cout << "GAME"; break;
	}
	std::cout << '\n';

	std::cout << "BuildMode ";
	switch (buildMode_)
	{
		case BuildMode::LIBS: std::cout << "LIBS"; break;
		case BuildMode::ENGINE: std::cout << "ENGINE"; break;
		case BuildMode::GAME: std::cout << "GAME"; break;
	}
	std::cout << '\n';
}
