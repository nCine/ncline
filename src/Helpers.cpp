#include <cassert>
#include <iostream>
#include "Helpers.h"
#include "CMakeCommand.h"
#include "Settings.h"
#include "Configuration.h"

#ifdef _WIN32
	#define _AMD64_
	#include <windef.h>
	#include <WinBase.h>
#else
	#include <cstdlib>
#endif

namespace {

const char *Red = "\033[31m";
const char *Green = "\033[32m";
const char *Yellow = "\033[33m";
const char *Cyan = "\033[36m";
const char *Blue = "\033[94m";
const char *EndColor = "\033[0m";

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

const char *Helpers::getEnvironment(const char *name)
{
#ifdef _WIN32
	const int MaxLength = 2048;
	static char buffer[MaxLength];
	const DWORD numChars = GetEnvironmentVariable(name, buffer, MaxLength);
	if (numChars == 0 || numChars > MaxLength)
		return nullptr;
	else
		return buffer;
#else
	return getenv(name);
#endif
}

bool Helpers::setEnvironment(const char *name, const char *value)
{
#ifdef _WIN32
	return SetEnvironmentVariable(name, value);
#else
	const int retVal = setenv(name, value, 1);
	return (retVal == 0);
#endif
}

bool Helpers::addDirToPath(const char *directory)
{
	const char *oldPath = getEnvironment("PATH");
	if (oldPath == nullptr)
		return false;

	std::string newPath(directory);
#ifdef _WIN32
	newPath.append(";");
#else
	newPath.append(":");
#endif
	newPath.append(oldPath);
	const bool retVal = setEnvironment("PATH", newPath.data());

	return retVal;
}

bool Helpers::checkMinVersion(unsigned int major, unsigned int minor, unsigned int patch, unsigned int minMajor, unsigned int minMinor, unsigned int minPatch)
{
	if (minMajor > major)
		return false;
	else if (minMajor == major && minMinor > minor)
		return false;
	else if (minMajor == major && minMinor == minor && minPatch > patch)
		return false;
	return true;
}

bool Helpers::checkMinVersion(const unsigned int version[3], unsigned int minMajor, unsigned int minMinor, unsigned int minPatch)
{
	return checkMinVersion(version[0], version[1], version[2], minMajor, minMinor, minPatch);
}

void Helpers::echo(const char *msg)
{
	std::cout << ":: " << msg << "\n" << std::flush;
}

void Helpers::info(const char *msg)
{
	std::cout << "-> ";

	if (config().withColors())
		std::cout << Blue;
	std::cout << msg;
	if (config().withColors())
		std::cout << EndColor;

	std::cout << "\n";
}

void Helpers::info(const char *msg1, const char *msg2)
{
	std::cout << "-> ";

	if (config().withColors())
		std::cout << Blue;
	std::cout << msg1;
	if (config().withColors())
		std::cout << EndColor;

	std::cout << msg2 << "\n";
}

void Helpers::error(const char *msg)
{
	std::cout << "!! ";

	if (config().withColors())
		std::cout << Red;
	std::cout << msg;
	if (config().withColors())
		std::cout << EndColor;

	std::cout << "\n";
}

void Helpers::error(const char *msg1, const char *msg2)
{
	std::cout << "!! ";

	if (config().withColors())
		std::cout << Red;
	std::cout << msg1;
	if (config().withColors())
		std::cout << EndColor;

	std::cout << msg2 << "\n";
}

void Helpers::buildDir(std::string &binaryDir, const Settings &settings)
{
	if (config().platform() == Configuration::Platform::EMSCRIPTEN)
		binaryDir += (settings.buildType() == Settings::BuildType::DEBUG) ? "-web-debug" : "-web-release";
	else
	{
		// nCine-android-libraries have a suffix based on the build type also on Windows
		if (CMakeCommand::generatorIsMultiConfig() &&
		    (config().platform() != Configuration::Platform::ANDROID || settings.target() != Settings::Target::LIBS))
			binaryDir += "-build";
		else
			binaryDir += (settings.buildType() == Settings::BuildType::DEBUG) ? "-debug" : "-release";
	}
}

std::string Helpers::gameRepositoryUrl(const std::string &gameName)
{
	assert(gameName.empty() == false);

	std::string repositoryUrl = "https://github.com/nCine/";
	repositoryUrl += gameName + ".git";
	return repositoryUrl;
}

std::string Helpers::gameArtifactsRepositoryUrl(const std::string &gameName)
{
	assert(gameName.empty() == false);

	std::string repositoryUrl = "https://github.com/nCine/";
	repositoryUrl += gameName + "-artifacts.git";
	return repositoryUrl;
}

std::string Helpers::gameDataRepositoryUrl(const std::string &gameName)
{
	assert(gameName.empty() == false);

	std::string repositoryUrl = "https://github.com/nCine/";
	repositoryUrl += gameName + "-data.git";
	return repositoryUrl;
}

std::string Helpers::gameArtifactsSourceDir(const std::string &gameName)
{
	assert(gameName.empty() == false);

	std::string sourceDir = gameName + "-artifacts";
	return sourceDir;
}

std::string Helpers::gameDataSourceDir(const std::string &gameName)
{
	assert(gameName.empty() == false);

	std::string sourceDir = gameName + "-data";
	return sourceDir;
}
