#pragma once

#include <string>

/// A class to execute a process and get its output
class Process
{
  public:
	enum class Echo
	{
		DISABLED,
		ENABLED
	};

	static bool executeCommand(const char *command) { return executeCommand(command, Echo::ENABLED); }
	static bool executeCommand(const char *command, Echo echoMode);
	static bool executeCommand(const char *command, std::string &output) { return executeCommand(command, output, Echo::ENABLED); }
	static bool executeCommand(const char *command, std::string &output, Echo echoMode);
};
