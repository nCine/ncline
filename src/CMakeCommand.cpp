#include <cassert>
#include <cstring>
#include <iostream>
#include <thread>
#include "CMakeCommand.h"
#include "Process.h"
#include "Configuration.h"

namespace {

const int MaxLength = 1024;
// TODO: Unify buffers
char buffer[MaxLength];

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

CMakeCommand::CMakeCommand(const char *executable)
    : found_(false), ninjaFound_(false), executable_(executable)
{
	output_.reserve(1024);

	snprintf(buffer, MaxLength, "%s --version", executable_.data());
	const bool executed = Process::executeCommand(buffer, output_, Process::Echo::DISABLED);
	if (executed)
	{
		found_ = sscanf(output_.data(), "cmake version %d.%d.%d", &version_[0], &version_[1], &version_[2]);
		findNinja();
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool CMakeCommand::configure(const char *srcDir, const char *binDir, const char *arguments)
{
	assert(found_);
	assert(srcDir);
	assert(binDir);
	output_.clear();

	snprintf(buffer, MaxLength, "%s -S \"%s\" -B \"%s\"", executable_.data(), srcDir, binDir);
	std::string configureCommand = buffer;

	if (platform())
		snprintf(buffer, MaxLength, " -G \"%s\" -A %s", generator(), platform());
	else
		snprintf(buffer, MaxLength, " -G \"%s\"", generator());
	configureCommand += buffer;

	if (arguments)
	{
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

const char *CMakeCommand::generator()
{
#ifdef __WIN32
	return "Visual Studio 15 2017";
#else
	if (ninjaFound_ && config().withNinja())
		return "Ninja";
	else
		return "Unix Makefiles";
#endif
}

const char *CMakeCommand::platform()
{
#ifdef __WIN32
	return "x64";
#else
	return nullptr;
#endif
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void CMakeCommand::findNinja()
{
	output_.clear();
	const bool executed = Process::executeCommand("ninja --version", output_, Process::Echo::DISABLED);
	if (executed)
		ninjaFound_ = sscanf(output_.data(), "%d.%d.%d", &ninjaVersion_[0], &ninjaVersion_[1], &ninjaVersion_[2]);
}
