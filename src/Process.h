#pragma once

#include <string>

/// A class to execute a process and retrieve its output
class Process
{
  public:
	enum class Echo
	{
		DISABLED,
		COMMAND_ONLY,
		ENABLED
	};

	enum class OverrideDryRun
	{
		DISABLED,
		ENABLED
	};

#ifdef _WIN32
	static void setupJobObject();
	static void detectPowerShell();
#endif

	static bool dryRun;
	static bool powerShell;

	static bool executeCommand(const char *command);
	static bool executeCommand(const char *command, Echo echoMode);
	static bool executeCommand(const char *command, std::string &output);
	static bool executeCommand(const char *command, std::string &output, Echo echoMode);
	static bool executeCommand(const char *command, std::string &output, Echo echoMode, OverrideDryRun overrideMode);

  private:
	static bool executeCommand(const char *command, std::string *output, Echo echoMode, OverrideDryRun overrideMode);
};
