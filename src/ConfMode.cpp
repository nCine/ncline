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

bool preferredCompiler(std::string &cmakeArguments)
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

bool buildType(std::string &cmakeArguments, const Settings &settings)
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

void configureLibraries(CMakeCommand &cmake, const Settings &settings)
{
	Helpers::info("Configure the libraries");

	std::string buildDir = Helpers::nCineLibrariesSourceDir();
	Helpers::buildDir(buildDir, settings);

	std::string arguments;
	preferredCompiler(arguments);
	buildType(arguments, settings);

	cmake.configure(Helpers::nCineLibrariesSourceDir(), buildDir.data(), arguments.empty() ? nullptr : arguments.data());
}

void configureEngine(CMakeCommand &cmake, const Settings &settings)
{
	Helpers::info("Configure the engine");

	std::string buildDir = Helpers::nCineSourceDir();
	Helpers::buildDir(buildDir, settings);

	std::string arguments;
	preferredCompiler(arguments);
	buildType(arguments, settings);

	cmake.configure(Helpers::nCineSourceDir(), buildDir.data(), arguments.empty() ? nullptr : arguments.data());
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
