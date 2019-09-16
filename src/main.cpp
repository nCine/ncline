#include "Configuration.h"
#include "Settings.h"
#include "GitCommand.h"
#include "CMakeCommand.h"
#include "Process.h"
#include "Helpers.h"

#include "DownloadMode.h"
#include "ConfMode.h"
#include "BuildMode.h"
#include "DistMode.h"

int main(int argc, char **argv)
{
	Settings settings;
	const bool parsed = settings.parseArguments(argc, argv);
	if (parsed == false)
		return EXIT_FAILURE;
	config().save();

	if (settings.mode() == Settings::Mode::SET)
		config().print();
	else if (settings.mode() != Settings::Mode::HELP &&
	         settings.mode() != Settings::Mode::VERSION)
	{
#ifdef _WIN32
		Process::setupJobObject();
#endif

		GitCommand git;
		if (git.found() == false)
			Helpers::error("Cannot find Git executable: ", git.executable().data());
		else
			Helpers::info("Git executable found: ", git.executable().data());

		CMakeCommand cmake;
		if (cmake.found() == false)
			Helpers::error("Cannot find CMake executable: ", cmake.executable().data());
		else
		{
			if (cmake.isUpdated() == false)
				Helpers::error("Old version of CMake executable found: ", cmake.executable().data());
			else
				Helpers::info("CMake executable found: ", cmake.executable().data());
		}

		if (config().withNinja())
		{
			if (cmake.ninjaFound() == false)
				Helpers::error("Cannot find Ninja executable: ", cmake.ninjaExecutable().data());
			else
				Helpers::info("Ninja executable found: ", cmake.ninjaExecutable().data());
		}

		bool gameNameIsMissing = false;
		if (settings.target() == Settings::Target::GAME && config().hasGameName() == false)
		{
			Helpers::error("No game name in the configuration");
			Helpers::info("Specify a game name with the 'set' command");
			gameNameIsMissing = true;
		}

		if (git.found() && cmake.found() && gameNameIsMissing == false)
		{
			switch (settings.mode())
			{
				case Settings::Mode::DOWNLOAD: DownloadMode::perform(git, cmake, settings); break;
				case Settings::Mode::CONF: ConfMode::perform(cmake, settings); break;
				case Settings::Mode::BUILD: BuildMode::perform(cmake, settings); break;
				case Settings::Mode::DIST: DistMode::perform(cmake, settings); break;
				default: break;
			}
		}
	}

	return EXIT_SUCCESS;
}
