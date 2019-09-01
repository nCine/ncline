#include <iostream>
#include "Process.h"

namespace {

char lineBuffer[1024];

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool Process::executeCommand(const char *command, Echo echoMode)
{
	if (echoMode == Echo::ENABLED)
		std::cout << command << "\n";

	FILE *fp = popen(command, "r");

	if (!fp)
	{
		std::cerr << "Cannot execute " << command;
		return false;
	}

	while (fgets(lineBuffer, 512, fp))
		std::cout << lineBuffer;

	const bool readToTheEnd = feof(fp);
	pclose(fp);

	return readToTheEnd;
}

bool Process::executeCommand(const char *command, std::string &output, Echo echoMode)
{
	if (echoMode == Echo::ENABLED)
		std::cout << command << "\n";

	FILE *fp = popen(command, "r");

	if (!fp)
	{
		std::cerr << "Cannot execute " << command;
		return false;
	}

	while (fgets(lineBuffer, 512, fp))
		output.append(lineBuffer);

	const bool readToTheEnd = feof(fp);
	pclose(fp);

	return readToTheEnd;
}
