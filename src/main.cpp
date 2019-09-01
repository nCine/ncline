#include <iostream>
#include "Configuration.h"
#include "Settings.h"
#include "GitCommand.h"
#include "CMakeCommand.h"

#include "DownloadMode.h"
#include "ConfMode.h"
#include "BuildMode.h"

int main(int argc, char **argv)
{
	Settings settings;
	const bool parsed = settings.parseArguments(argc, argv);
	//settings.print();
	config().save();

	if (parsed == false)
		return EXIT_FAILURE;

	GitCommand git("git");
	if (git.found() == false)
		std::cerr << "Cannot found git executable: \"" << git.executable() << "\"";

	CMakeCommand cmake("cmake");
	if (cmake.found() == false)
		std::cerr << "Cannot found cmake executable: \"" << cmake.executable() << "\"";

	switch (settings.mode())
	{
		case Settings::Mode::DOWNLOAD: DownloadMode::perform(git, settings); break;
		case Settings::Mode::CONF: ConfMode::perform(cmake, settings); break;
		case Settings::Mode::BUILD: BuildMode::perform(cmake, settings); break;
		default: break;
	}

	return EXIT_SUCCESS;
}
