#include <cassert>
#include <cstring>
#include <thread>
#include "CMakeCommand.h"
#include "Process.h"
#include "FileSystem.h"
#include "Configuration.h"
#include "Helpers.h"

using fs = FileSystem;

namespace {

const int MaxLength = 1024;
// TODO: Unify buffers
char buffer[MaxLength];

#ifdef _WIN32
const char *vsVersionToGeneratorString(int version)
{
	if (version == 2017)
		return "Visual Studio 15 2017";
	else
		return "Visual Studio 16 2019";
}
#endif

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

CMakeCommand::CMakeCommand()
    : found_(false), ninjaFound_(false)
{
	output_.reserve(1024);

	if (config().cmakeExecutable(executable_) == false)
	{
		if (checkPredefinedLocations() == false)
			executable_ = "cmake";
	}

	if (executable_.find(' ') != std::string::npos)
		executable_ = "\"" + executable_ + "\"";

	snprintf(buffer, MaxLength, "%s --version", executable_.data());
	const bool executed = Process::executeCommand(buffer, output_, Process::Echo::DISABLED);
	if (executed)
	{
		found_ = sscanf(output_.data(), "cmake version %u.%u.%u", &version_[0], &version_[1], &version_[2]);

		if (config().withNinja())
			findNinja();
	}

	if (config().emcmakeExecutable(emcmakeExecutable_) == false)
		emcmakeExecutable_ = "emcmake";
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool CMakeCommand::generatorIsMultiConfig()
{
	if (config().withEmscripten())
		return false;

#ifdef _WIN32
	if (config().withMinGW())
		return false;
	else
		return true;
#else
	return false;
#endif
}

bool CMakeCommand::configure(const char *srcDir, const char *binDir, const char *arguments)
{
	assert(found_);
	assert(srcDir);
	assert(binDir);
	output_.clear();

	std::string configureCommand;
	if (config().withEmscripten())
		configureCommand = emcmakeExecutable_ + " " + executable_;
	else
		configureCommand = executable_;

	snprintf(buffer, MaxLength, " -S \"%s\" -B \"%s\"", srcDir, binDir);
	configureCommand += buffer;

	if (config().withEmscripten() == false)
	{
		if (platform())
			snprintf(buffer, MaxLength, " -G \"%s\" -A %s", generator(), platform());
		else
			snprintf(buffer, MaxLength, " -G \"%s\"", generator());
		configureCommand += buffer;
	}

	if (arguments)
	{
		if (arguments[0] != ' ')
			configureCommand.append(" ");
		configureCommand += arguments;
	}

	const bool executed = Process::executeCommand(configureCommand.data());
	return executed;
}

bool CMakeCommand::configure(const char *srcDir, const char *binDir)
{
	return configure(srcDir, binDir, nullptr);
}

bool CMakeCommand::build(const char *buildDir, const char *config, const char *target)
{
	assert(found_);
	assert(buildDir);
	output_.clear();

	snprintf(buffer, MaxLength, "%s --build \"%s\" -j %u", executable_.data(), buildDir, std::thread::hardware_concurrency());
	std::string buildCommand = buffer;

	if (config)
	{
		snprintf(buffer, MaxLength, " --config %s", config);
		buildCommand += buffer;
	}

	if (target)
	{
		snprintf(buffer, MaxLength, " --target %s", target);
		buildCommand += buffer;
	}

	const bool executed = Process::executeCommand(buildCommand.data());
	return executed;
}

bool CMakeCommand::isUpdated() const
{
	assert(found_);
	return Helpers::checkMinVersion(version_, 3, 13, 0);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

const char *CMakeCommand::generator() const
{
#ifdef _WIN32
	if (config().withMinGW())
		return "MSYS Makefiles";
	else
		return vsVersionToGeneratorString(config().vsVersion());
#else
	if (ninjaFound_ && config().withNinja())
		return "Ninja";
	else
		return "Unix Makefiles";
#endif
}

const char *CMakeCommand::platform() const
{
#ifdef _WIN32
	if (generatorIsVisualStudio())
		return "x64";
	else
#endif
		return nullptr;
}

bool CMakeCommand::checkPredefinedLocations()
{
	bool isAccessible = false;

#if defined(_WIN32)
	std::string programsToCmake = "CMake/bin/cmake.exe";

	if (fs::canAccess(fs::joinPath(Helpers::getEnvironment("ProgramW6432"), programsToCmake).data()))
	{
		executable_ = fs::joinPath(Helpers::getEnvironment("ProgramW6432"), programsToCmake);
		isAccessible = true;
	}
	else if (fs::canAccess(fs::joinPath(Helpers::getEnvironment("ProgramFiles"), programsToCmake).data()))
	{
		executable_ = fs::joinPath(Helpers::getEnvironment("ProgramFiles"), programsToCmake);
		isAccessible = true;
	}
#elif defined(__APPLE__)
	if (fs::canAccess("/Applications/CMake.app/Contents/bin/cmake"))
	{
		executable_ = "/Applications/CMake.app/Contents/bin/cmake";
		isAccessible = true;
	}
#endif

	return isAccessible;
}

void CMakeCommand::findNinja()
{
	output_.clear();

	if (config().ninjaExecutable(ninjaExecutable_) == false)
		ninjaExecutable_ = "ninja";

	if (ninjaExecutable_.find(' ') != std::string::npos)
		ninjaExecutable_ = "\"" + ninjaExecutable_ + "\"";

	snprintf(buffer, MaxLength, "%s --version", ninjaExecutable_.data());
	const bool executed = Process::executeCommand(buffer, output_, Process::Echo::DISABLED);
	if (executed)
		ninjaFound_ = sscanf(output_.data(), "%u.%u.%u", &ninjaVersion_[0], &ninjaVersion_[1], &ninjaVersion_[2]);
}
