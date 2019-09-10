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
	                    option("-emscripten").call([] { config().setWithEmscripten(true); }) |
	                    option("-no-emscripten").call([] { config().setWithEmscripten(false); })
	                ).doc("(do not) use Emscripten to compile for the web"),
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
#ifndef __APPLE__
	                (
	                    option("-gcc").call([] { config().setCompiler(Configuration::Compiler::GCC); }) |
	                    option("-clang").call([] { config().setCompiler(Configuration::Compiler::CLANG); })
	                ).doc("prefer GCC or Clang to compile on Linux and MinGW"),
#endif
	                (
	                    option("-colors").call([] { config().setWithColors(true); }) |
	                    option("-no-colors").call([] { config().setWithColors(false); })
	                ).doc("(do not) use shell colors in the output"),
	                option("-git-exe") & value("executable").call([&](const std::string &gitExe) { config().setGitExecutable(gitExe); }).doc("set the Git command executable"),
	                option("-cmake-exe") & value("executable").call([&](const std::string &cmakeExe) { config().setCMakeExecutable(cmakeExe); }).doc("set the CMake command executable"),
	                option("-ninja-exe") & value("executable").call([&](const std::string &ninjaExe) { config().setNinjaExecutable(ninjaExe); }).doc("set the Ninja command executable"),
	                option("-emcmake-exe") & value("executable").call([&](const std::string &emcmakeExe) { config().setEmcmakeExecutable(emcmakeExe); }).doc("set the Emscripten emcmake command executable"),
	                option("-cmake-args") & value("args").call([&](const std::string &cmakeArgs) { config().setEngineCMakeArguments(cmakeArgs); }).doc("additional CMake arguments to configure the engine"),
	                option("-branch") & value("name").call([&](const std::string &branchName) { config().setBranchName(branchName); }).doc("branch name for engine and projects"),
	                option("-ncine-dir") & value("path").call([&](const std::string &directory) { config().setEngineDir(directory); }).doc("path to the CMake script directory inside a compiled or installed engine"),
	                option("-game") & value("name").call([&](const std::string &gameName) { config().setGameName(gameName); }).doc("name of the game project")
	               );

	auto downloadMode = (command("download").set(mode_, Mode::DOWNLOAD).doc("download mode"),
	                     command("libs").set(target_, Target::LIBS) |
	                     command("engine").set(target_, Target::ENGINE) |
	                     command("game").set(target_, Target::GAME),
	                     option("-artifact").set(downloadArtifact_, true).doc("download the C.I. compiled artifact instead of source code"));

	auto confMode = (command("conf").set(mode_, Mode::CONF).doc("configuration mode"),
	                 command("libs").set(target_, Target::LIBS) |
	                 command("engine").set(target_, Target::ENGINE) |
	                 command("game").set(target_, Target::GAME));

	auto buildMode = (command("build").set(mode_, Mode::BUILD).doc("build mode"),
	                  command("libs").set(target_, Target::LIBS) |
	                  command("engine").set(target_, Target::ENGINE) |
	                  command("game").set(target_, Target::GAME));

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
