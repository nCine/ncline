#include <cassert>
#include <iostream>
#include <string>
#include "Process.h"
#include "Helpers.h"

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif

namespace {

const int MaxLength = 1024;
char buffer[MaxLength];

FILE *popenWrapper(const char *command, const char *mode)
{
#ifdef _WIN32
	// Quoting the command string before passing it to `_popen()`
	snprintf(buffer, MaxLength, "\"%s\"", command);
	return _popen(buffer, mode);
#else
	return popen(command, mode);
#endif
}

int pcloseWrapper(FILE *stream)
{
#ifdef _WIN32
	return _pclose(stream);
#else
	return pclose(stream);
#endif
}

#ifdef _WIN32
HANDLE jobObject_ = 0;

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		case CTRL_C_EVENT:
			Beep(750, 300);
			TerminateJobObject(jobObject_, EXIT_FAILURE);
			return TRUE;
		default:
			return FALSE;
	}
}
#endif

}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool Process::dryRun = false;
#ifdef _WIN32
bool Process::powerShell = false;
#endif

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

#ifdef _WIN32
void Process::setupJobObject()
{
	jobObject_ = CreateJobObjectA(nullptr, "ncline");
	const BOOL successAssign = AssignProcessToJobObject(jobObject_, GetCurrentProcess());
	assert(successAssign);

	const BOOL successHandler = SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
	assert(successHandler);
}

void Process::detectPowerShell()
{
	const char *hostName = Helpers::getEnvironment("host.name");
	if (hostName)
	{
		std::string psHostName = "ConsoleHost";
		if (psHostName == hostName)
			powerShell = true;
	}
}
#endif

bool Process::executeCommand(const char *command)
{
	return executeCommand(command, nullptr, Echo::ENABLED);
}

bool Process::executeCommand(const char *command, Echo echoMode)
{
	return executeCommand(command, nullptr, echoMode);
}

bool Process::executeCommand(const char *command, std::string &output)
{
	return executeCommand(command, &output, Echo::ENABLED);
}

bool Process::executeCommand(const char *command, std::string &output, Echo echoMode)
{
	return executeCommand(command, &output, echoMode);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool Process::executeCommand(const char *command, std::string *output, Echo echoMode)
{
	assert(command);

	if (echoMode != Echo::DISABLED)
		Helpers::echo(command);

	if (dryRun)
		return true;

	FILE *fp = popenWrapper(command, "r");

	if (!fp)
	{
		std::cerr << "Cannot execute " << command;
		return false;
	}

	if (output)
		output->clear();

	while (fgets(buffer, MaxLength, fp))
	{
		if (output)
			output->append(buffer);
		if (echoMode == Echo::ENABLED)
			std::cout << buffer << std::flush;
	}

	if (feof(fp))
		return (pcloseWrapper(fp) == EXIT_SUCCESS);
	else
		return false;
}
