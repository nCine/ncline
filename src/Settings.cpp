#include <iostream>
#include <clipp.h>
#include "Settings.h"
#include "Configuration.h"
#include "CMakeCommand.h"
#include "Process.h"
#include "version.h"

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
	auto setMode = (command("set").set(mode_, Mode::SET).doc("settings mode"),
	                (
	                    option("-desktop").call([] { config().setPlatform(Configuration::Platform::DESKTOP); }) |
	                    option("-android").call([] { config().setPlatform(Configuration::Platform::ANDROID); }) |
	                    option("-emscripten").call([] { config().setPlatform(Configuration::Platform::EMSCRIPTEN); })
	                ).doc("compile for the host platform or cross-compile for Android or Emscripten"),
	                (
	                    option("-ninja").call([] { config().setWithNinja(true); }) |
	                    option("-no-ninja").call([] { config().setWithNinja(false); })
	                ).doc("(do not) prefer Ninja as a CMake generator"),
#ifdef _WIN32
	                ((
	                    option("-mingw").call([] { config().setWithMinGW(true); }) |
	                    option("-no-mingw").call([] { config().setWithMinGW(false); })
	                ).doc("(do not) use MinGW/MSYS environment to configure and build"),
	                (
	                    option("-vs2017").call([] { config().setVsVersion(2017); }) |
	                    option("-vs2019").call([] { config().setVsVersion(2019); })
	                ).doc("prefer Visual Studio 2017 or 2019 to configure and build")).doc("Windows only settings"),
#endif
#ifndef __APPLE__
	                (
	                    option("-gcc").call([] { config().setCompiler(Configuration::Compiler::GCC); }) |
	                    option("-clang").call([] { config().setCompiler(Configuration::Compiler::CLANG); })
	                ).doc("prefer GCC or Clang to compile on Linux and MinGW/MSYS"),
#endif
	                ((
	                    option("-armeabi-v7a").call([] { config().setAndroidArch(Configuration::AndroidArch::ARMEABI_V7A); }) |
	                    option("-arm64-v8a").call([] { config().setAndroidArch(Configuration::AndroidArch::ARM64_V8A); }) |
	                    option("-x86_64").call([] { config().setAndroidArch(Configuration::AndroidArch::X86_64); })
	                ).doc("choose one of the supported Android architectures"),
	                (option("-sdk-dir") & value("path").call([&](const std::string &directory) { config().setAndroidSdkDir(directory); })).doc("set the Android SDK directory"),
	                (option("-ndk-dir") & value("path").call([&](const std::string &directory) { config().setAndroidNdkDir(directory); })).doc("set the Android NDK directory"),
	                (option("-gradle-dir") & value("path").call([&](const std::string &directory) { config().setGradleDir(directory); })).doc("set the Gradle home directory")).doc("Android only settings"),
	                (
	                    option("-colors").call([] { config().setWithColors(true); }) |
	                    option("-no-colors").call([] { config().setWithColors(false); })
	                ).doc("(do not) use colors in the terminal output"),
	                (option("-git-exe") & value("executable").call([&](const std::string &gitExe) { config().setGitExecutable(gitExe); })).doc("set the Git command executable"),
	                (option("-cmake-exe") & value("executable").call([&](const std::string &cmakeExe) { config().setCMakeExecutable(cmakeExe); })).doc("set the CMake command executable"),
	                (option("-ninja-exe") & value("executable").call([&](const std::string &ninjaExe) { config().setNinjaExecutable(ninjaExe); })).doc("set the Ninja command executable"),
	                (option("-emcmake-exe") & value("executable").call([&](const std::string &emcmakeExe) { config().setEmcmakeExecutable(emcmakeExe); })).doc("set the Emscripten emcmake command executable"),
	                (option("-doxygen-exe") & value("executable").call([&](const std::string &doxygenExe) { config().setDoxygenExecutable(doxygenExe); })).doc("set the Doxygen command executable"),
	                (option("-prefix-path") & value("path").call([&](const std::string &directory) { config().setCMakePrefixPath(directory); })).doc("set the CMAKE_PREFIX_PATH variable for the engine"),
	                (option("-cmake-args") & value("args").call([&](const std::string &cmakeArgs) { config().setEngineCMakeArguments(cmakeArgs); })).doc("additional CMake arguments to configure the engine"),
	                (option("-branch") & value("name").call([&](const std::string &branchName) { config().setBranchName(branchName); })).doc("branch name for engine and projects"),
	                (option("-ncine-dir") & value("path").call([&](const std::string &directory) { config().setEngineDir(directory); })).doc("path to the CMake script directory inside a compiled or installed engine"),
	                (option("-game") & value("name").call([&](const std::string &gameName) { config().setGameName(gameName); })).doc("name of the game project")
	               );

	auto downloadMode = (command("download").set(mode_, Mode::DOWNLOAD).doc("download mode"),
	                     (command("libs").set(target_, Target::LIBS) |
	                     command("engine").set(target_, Target::ENGINE) |
	                     command("game").set(target_, Target::GAME)).doc("choose what to download"),
	                     option("-artifact").set(downloadArtifact_, true).doc("download the C.I. compiled artifact instead of source code"));

	auto confMode = (command("conf").set(mode_, Mode::CONF).doc("configuration mode"),
	                 (command("libs").set(target_, Target::LIBS) |
	                 command("engine").set(target_, Target::ENGINE) |
	                 command("game").set(target_, Target::GAME)).doc("choose what to configure"));

	auto buildMode = (command("build").set(mode_, Mode::BUILD).doc("build mode"),
	                  (command("libs").set(target_, Target::LIBS) |
	                  command("engine").set(target_, Target::ENGINE) |
	                  command("game").set(target_, Target::GAME)).doc("choose what to build"));

	auto distMode = (command("dist").set(mode_, Mode::DIST).doc("distribution mode"),
	                 (command("engine").set(target_, Target::ENGINE) |
	                 command("game").set(target_, Target::GAME)).doc("choose what to distribute"));

	if (CMakeCommand::generatorIsMultiConfig() == false)
		confMode.push_back((command("debug").set(buildType_, BuildType::DEBUG) | command("release").set(buildType_, BuildType::RELEASE)).doc("choose debug or release build type"));
	else
	{
		// When compiling Android with the Visual Studio generator the CMAKE_BUILD_TYPE variable needs to be set
		if (config().platform() == Configuration::Platform::ANDROID)
			confMode.push_back((command("debug").set(buildType_, BuildType::DEBUG) | command("release").set(buildType_, BuildType::RELEASE)).doc("choose debug or release build type for Android"));
		buildMode.push_back((command("debug").set(buildType_, BuildType::DEBUG) | command("release").set(buildType_, BuildType::RELEASE)).doc("choose debug or release build configuration"));
	}

	auto cleanOption = option("-clean").set(clean_, true).doc("remove an existing build directory before recreating it");
	confMode.push_back(cleanOption);
	distMode.push_back(cleanOption);

	auto dryRunOption = option("-dry-run").set(Process::dryRun, true).doc("show which commands to execute without executing them");
	downloadMode.push_back(dryRunOption);
	confMode.push_back(dryRunOption);
	buildMode.push_back(dryRunOption);
	distMode.push_back(dryRunOption);

	auto cli = ((setMode | downloadMode | confMode | buildMode | distMode |
	             command("--help").set(mode_, Mode::HELP).doc("show help") |
	             command("--version").set(mode_, Mode::VERSION).doc("show version")));
	// clang-format on

	bool parsed = false;
	if (parse(argc, argv, cli))
	{
		parsed = true;
		if (mode_ == Mode::HELP)
		{
			auto fmt = doc_formatting{}.first_column(7).doc_column(15).last_column(99);
			std::cout << make_man_page(cli, programName, fmt)
			                 .prepend_section("DESCRIPTION", "The nCine command line tool. A tool to automate the download, configuration and compilation processes for the engine and its accompanying projects.");
		}
		else if (mode_ == Mode::VERSION)
			std::cout << programName << " version " << VersionStrings::Version << "\n";
	}
	else
		std::cout << usage_lines(cli, programName) << '\n';

	return parsed;
}
