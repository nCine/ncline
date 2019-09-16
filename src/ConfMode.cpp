#include <cassert>
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
	if (config().platform() == Configuration::Platform::EMSCRIPTEN)
		return false;

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

	// When compiling Android with the Visual Studio generator the CMAKE_BUILD_TYPE variable needs to be set
	if (CMakeCommand::generatorIsMultiConfig() == false || config().platform() == Configuration::Platform::ANDROID)
	{
		cmakeArguments += " -D CMAKE_BUILD_TYPE=";
		cmakeArguments += settingsToBuildTypeString(settings.buildType());
		argumentsAdded = true;
	}

	return argumentsAdded;
}

bool androidArchArg(std::string &cmakeArguments)
{
	if (config().platform() != Configuration::Platform::ANDROID)
		return false;

	bool argumentsAdded = true;

	switch (config().androidArch())
	{
		case Configuration::AndroidArch::ARMEABI_V7A:
			cmakeArguments += " -D ARCH=armeabi-v7a";
			break;
		case Configuration::AndroidArch::ARM64_V8A:
		case Configuration::AndroidArch::UNSPECIFIED:
			cmakeArguments += " -D ARCH=arm64-v8a";
			break;
		case Configuration::AndroidArch::X86_64:
			cmakeArguments += " -D ARCH=x86_64";
			break;
	}

	return argumentsAdded;
}

bool engineAndroidArg(std::string &cmakeArguments)
{
	bool argumentsAdded = false;

	if (config().platform() == Configuration::Platform::ANDROID)
	{
		cmakeArguments += " -D NCINE_BUILD_ANDROID=ON";
		argumentsAdded = true;
	}

	return argumentsAdded;
}

bool gameAndroidArg(std::string &cmakeArguments)
{
	bool argumentsAdded = false;

	if (config().platform() == Configuration::Platform::ANDROID)
	{
		cmakeArguments += " -D PACKAGE_BUILD_ANDROID=ON -D PACKAGE_ASSEMBLE_APK=ON";
		argumentsAdded = true;
	}

	return argumentsAdded;
}

bool prefixPathArg(std::string &cmakeArguments)
{
	std::string arguments;
	const bool argumentsAdded = config().cmakePrefixPath(arguments);
	if (argumentsAdded)
		cmakeArguments += " -D CMAKE_PREFIX_PATH=" + arguments;

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

void configureAndroidLibraries(CMakeCommand &cmake, const Settings &settings)
{
	Helpers::info("Configure the Android libraries");

	std::string buildDir = Helpers::nCineAndroidLibrariesSourceDir();
	Helpers::buildDir(buildDir, settings);

	std::string arguments;
	androidArchArg(arguments);
	buildTypeArg(arguments, settings);

#ifdef _WIN32
	cmake.configure(Helpers::nCineAndroidLibrariesSourceDir(), buildDir.data(), "NMake Makefiles", nullptr, arguments.empty() ? nullptr : arguments.data());
#else
	cmake.configure(Helpers::nCineAndroidLibrariesSourceDir(), buildDir.data(), arguments.empty() ? nullptr : arguments.data());
#endif
}

void configureLibraries(CMakeCommand &cmake, const Settings &settings)
{
	Helpers::info("Configure the libraries");

	std::string buildDir = Helpers::nCineLibrariesSourceDir();
	Helpers::buildDir(buildDir, settings);

	std::string arguments;
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
	preferredCompilerArgs(arguments);
	buildTypeArg(arguments, settings);
	engineAndroidArg(arguments);
	prefixPathArg(arguments);
	additionalArgs(arguments);

	cmake.configure(Helpers::nCineSourceDir(), buildDir.data(), arguments.empty() ? nullptr : arguments.data());
}

void configureGame(CMakeCommand &cmake, const Settings &settings, const std::string &gameName)
{
	Helpers::info("Configure the game: ", gameName.data());

	std::string buildDir = gameName;
	Helpers::buildDir(buildDir, settings);

	std::string arguments;
	preferredCompilerArgs(arguments);
	buildTypeArg(arguments, settings);
	gameAndroidArg(arguments);
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
			if (config().platform() == Configuration::Platform::ANDROID)
				configureAndroidLibraries(cmake, settings);
			else
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
