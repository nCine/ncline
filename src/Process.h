#pragma once

/// A class to execute a process and retrieve its output
class Process
{
  public:
	enum class Echo
	{
		DISABLED,
		ENABLED
	};

#ifdef _WIN32
	static void setupJobObject();
#endif

	static bool executeCommand(const char *command);
	static bool executeCommand(const char *command, Echo echoMode);
	static bool executeCommand(const char *command, std::string &output);
	static bool executeCommand(const char *command, std::string &output, Echo echoMode);

  private:
	static bool executeCommand(const char *command, std::string *output, Echo echoMode);
};
