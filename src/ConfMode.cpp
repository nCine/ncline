#include <cassert>
#include <iostream>
#include "ConfMode.h"
#include "CMakeCommand.h"
#include "Settings.h"
#include "Configuration.h"
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

bool preferredCompilerArgs(std::string &cmakeArguments)
{
	bool argumentsAdded = true;

	switch (config().compiler())
	{
		case Configuration::Compiler::GCC:
			cmakeArguments += " -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++";
			break;
		case Configuration::Compiler::CLANG:
			cmakeArguments += " -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++";
			break;
		case Configuration::Compiler::UNSPECIFIED:
			argumentsAdded = false;
			break;
	}

	return argumentsAdded;
}

bool buildTypeArg(std::string &cmakeArguments, const Settings &settings)
{
	bool argumentsAdded = false;

	if (CMakeCommand::generatorIsMultiConfig() == false)
	{
		cmakeArguments += " -D CMAKE_BUILD_TYPE=";
		cmakeArguments += settingsToBuildTypeString(settings.buildType());
		argumentsAdded = true;
	}

	return argumentsAdded;
}

bool additionalArgs(std::string &cmakeArguments)
{
	std::string arguments;
	const bool argumentsAdded = config().engineCMakeArguments(arguments);
	if (argumentsAdded)
		cmakeArguments += " " + arguments;

	return argumentsAdded;
}

bool ncineDirArg(std::string &cmakeArguments)
{
	std::string arguments;
	const bool argumentsAdded = config().engineDir(arguments);
	if (argumentsAdded)
		cmakeArguments += " -D nCine_DIR=" + arguments;

	return argumentsAdded;
}

void configureLibraries(CMakeCommand &cmake, const Settings &settings)
{
	Helpers::info("Configure the libraries");

	std::string buildDir = Helpers::nCineLibrariesSourceDir();
	Helpers::buildDir(buildDir, settings);

	std::string arguments;
	if (config().withEmscripten() == false)
		preferredCompilerArgs(arguments);
	buildTypeArg(arguments, settings);

	cmake.configure(Helpers::nCineLibrariesSourceDir(), buildDir.data(), arguments.empty() ? nullptr : arguments.data());
}

void configureEngine(CMakeCommand &cmake, const Settings &settings)
{
	Helpers::info("Configure the engine");

	std::string buildDir = Helpers::nCineSourceDir();
	Helpers::buildDir(buildDir, settings);

	std::string arguments;
	if (config().withEmscripten() == false)
		preferredCompilerArgs(arguments);
	buildTypeArg(arguments, settings);
	additionalArgs(arguments);

	cmake.configure(Helpers::nCineSourceDir(), buildDir.data(), arguments.empty() ? nullptr : arguments.data());
}

void configureGame(CMakeCommand &cmake, const Settings &settings, const std::string &gameName)
{
	Helpers::info("Configure the game: ", gameName.data());

	std::string buildDir = gameName;
	Helpers::buildDir(buildDir, settings);

	std::string arguments;
	if (config().withEmscripten() == false)
		preferredCompilerArgs(arguments);
	buildTypeArg(arguments, settings);
	ncineDirArg(arguments);

	cmake.configure(gameName.data(), buildDir.data(), arguments.empty() ? nullptr : arguments.data());
}

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ConfMode::perform(CMakeCommand &cmake, const Settings &settings)
{
	assert(settings.mode() == Settings::Mode::CONF);

	switch (settings.target())
	{
		case Settings::Target::LIBS:
			configureLibraries(cmake, settings);
			break;
		case Settings::Target::ENGINE:
			configureEngine(cmake, settings);
			break;
		case Settings::Target::GAME:
		{
			std::string gameName;
			config().gameName(gameName);

			configureGame(cmake, settings, gameName);
			break;
		}
	}
}
