#include <cassert>
#include <thread>
#include "CMakeCommand.h"
#include "Process.h"
#include "FileSystem.h"
#include "Configuration.h"
#include "Helpers.h"

namespace {

const int MaxLength = 1024;
char buffer[MaxLength];

#ifdef _WIN32
const char *vsVersionToGeneratorString(int version)
{
	if (version == 2022)
		return "Visual Studio 17 2022";
	else if (version == 2019)
		return "Visual Studio 16 2019";
	else if (version == 2017)
		return "Visual Studio 15 2017";
	else
		return "Visual Studio 17 2022";
}
#endif

#ifdef __APPLE__
const char *macosVersionToDeploymentTarget(int version)
{
	if (version == 14)
		return "CMAKE_OSX_DEPLOYMENT_TARGET=14";
	else if (version == 12)
		return "CMAKE_OSX_DEPLOYMENT_TARGET=12";
	else
		return "CMAKE_OSX_DEPLOYMENT_TARGET=12";
}
#endif

bool checkDoxygenPredefinedLocations(std::string &executable)
{
	bool isAccessible = false;

#if defined(_WIN32)
	std::string programsToDoxygen = "doxygen/bin/doxygen.exe";

	if (fs::canAccess(fs::joinPath(Helpers::getEnvironment("ProgramW6432"), programsToDoxygen).data()))
	{
		executable = fs::joinPath(Helpers::getEnvironment("ProgramW6432"), programsToDoxygen);
		isAccessible = true;
	}
	else if (fs::canAccess(fs::joinPath(Helpers::getEnvironment("ProgramFiles"), programsToDoxygen).data()))
	{
		executable = fs::joinPath(Helpers::getEnvironment("ProgramFiles"), programsToDoxygen);
		isAccessible = true;
	}
#elif defined(__APPLE__)
	if (fs::canAccess("/Applications/Doxygen.app/Contents/Resources/doxygen"))
	{
		executable = "/Applications/Doxygen.app/Contents/Resources/doxygen";
		isAccessible = true;
	}
#endif

	return isAccessible;
}

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
	const bool executed = Process::executeCommand(buffer, output_, Process::Echo::DISABLED, Process::OverrideDryRun::ENABLED);
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
	if (config().platform() == Configuration::Platform::EMSCRIPTEN)
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

const char *CMakeCommand::removeFile()
{
	assert(found_);
	if (Helpers::checkMinVersion(version_, 3, 17, 0))
		return "rm ";
	else
		return "remove ";
}

const char *CMakeCommand::removeDir()
{
	assert(found_);
	if (Helpers::checkMinVersion(version_, 3, 17, 0))
		return "rm -r ";
	else
		return "remove_directory ";
}

bool CMakeCommand::toolsMode(const char *command)
{
	assert(found_);
	assert(command);

	snprintf(buffer, MaxLength, "%s -E %s", executable_.data(), command);
	const bool executed = Process::executeCommand(buffer);
	return executed;
}

bool CMakeCommand::configure(const char *srcDir, const char *binDir, const char *generator, const char *platform, const char *arguments)
{
	assert(found_);
	assert(srcDir);
	assert(binDir);

	std::string configureCommand;
	if (config().platform() == Configuration::Platform::EMSCRIPTEN)
		configureCommand = emcmakeExecutable_ + " " + executable_;
	else
		configureCommand = executable_;

	snprintf(buffer, MaxLength, " -S \"%s\" -B \"%s\"", srcDir, binDir);
	configureCommand += buffer;

	if (generator)
	{
		if (platform)
			snprintf(buffer, MaxLength, " -G \"%s\" -A %s", generator, platform);
		else
			snprintf(buffer, MaxLength, " -G \"%s\"", generator);
		configureCommand += buffer;
	}

#ifdef __APPLE__
	snprintf(buffer, MaxLength, " -D %s", macosVersionToDeploymentTarget(config().macosVersion()));
	configureCommand += buffer;
#endif

	if (arguments)
	{
		if (arguments[0] != ' ')
			configureCommand.append(" ");
		configureCommand += arguments;
	}

	const bool executed = Process::executeCommand(configureCommand.data());
	return executed;
}

bool CMakeCommand::configure(const char *srcDir, const char *binDir, const char *arguments)
{
	return configure(srcDir, binDir, generator(), platform(), arguments);
}

bool CMakeCommand::configure(const char *srcDir, const char *binDir)
{
	return configure(srcDir, binDir, nullptr);
}

bool CMakeCommand::build(const char *buildDir, const char *config, const char *target)
{
	assert(found_);
	assert(buildDir);

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

void CMakeCommand::addAndroidNdkDirToPath()
{
	std::string androidNdkDir;
	if (config().androidNdkDir(androidNdkDir))
	{
		if (fs::isDirectory(androidNdkDir.data()))
		{
			Helpers::info("Set the ANDROID_NDK_HOME environment variable to: ", androidNdkDir.data());
			Helpers::setEnvironment("ANDROID_NDK_HOME", androidNdkDir.data());
		}
		else
			Helpers::error("The specified Android NDK directory does not exists: ", androidNdkDir.data());
	}
}

void CMakeCommand::addNMakeDirToPath()
{
#ifdef _WIN32
	std::string nmakeExecutable = findNMake();
	if (nmakeExecutable.empty() == false)
	{
		std::string nmakeDir = fs::dirName(nmakeExecutable.data());
		if (nmakeDir.empty() == false)
		{
			Helpers::info("Add NMake directory to path: ", nmakeDir.data());
			Helpers::addDirToPath(nmakeDir.data());
		}
	}
	else
		Helpers::error("Cannot find NMake executable");
#endif
}

void CMakeCommand::addAndroidSdkDirToPath()
{
	std::string androidSdkDir;
	if (config().androidSdkDir(androidSdkDir))
	{
		if (fs::isDirectory(androidSdkDir.data()))
		{
			Helpers::info("Set the ANDROID_HOME environment variable to: ", androidSdkDir.data());
			Helpers::setEnvironment("ANDROID_HOME", androidSdkDir.data());
		}
		else
			Helpers::error("The specified Android SDK directory does not exists: ", androidSdkDir.data());
	}
}

void CMakeCommand::addGradleDirToPath()
{
	std::string gradleDir;
	if (config().gradleDir(gradleDir))
	{
		gradleDir = fs::joinPath(gradleDir, "bin");
		if (fs::isDirectory(gradleDir.data()))
		{
			Helpers::info("Add Gradle binary directory to path: ", gradleDir.data());
			Helpers::addDirToPath(gradleDir.data());
		}
		else
			Helpers::error("The specified Gradle binary directory does not exists: ", gradleDir.data());
	}
}

void CMakeCommand::addDoxygenDirToPath()
{
	// Allow CMake to find doxygen if it's not already in the path
	std::string doxygenDir;
	if (config().doxygenExecutable(doxygenDir) == false)
		checkDoxygenPredefinedLocations(doxygenDir);

	doxygenDir = fs::dirName(doxygenDir.data());
	if (doxygenDir.empty() == false && doxygenDir != ".")
	{
		doxygenDir = fs::absolutePath(doxygenDir.data());
		Helpers::info("Add Doxygen directory to path: ", doxygenDir.data());
		Helpers::addDirToPath(doxygenDir.data());
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

const char *CMakeCommand::generator() const
{
	if (config().platform() == Configuration::Platform::EMSCRIPTEN)
		return nullptr;

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
	if (config().ninjaExecutable(ninjaExecutable_) == false)
		ninjaExecutable_ = "ninja";

	if (ninjaExecutable_.find(' ') != std::string::npos)
		ninjaExecutable_ = "\"" + ninjaExecutable_ + "\"";

	snprintf(buffer, MaxLength, "%s --version", ninjaExecutable_.data());
	const bool executed = Process::executeCommand(buffer, output_, Process::Echo::DISABLED);
	if (executed)
		ninjaFound_ = sscanf(output_.data(), "%u.%u.%u", &ninjaVersion_[0], &ninjaVersion_[1], &ninjaVersion_[2]);
}

std::string CMakeCommand::findNMake()
{
	std::string nmakeExcutable;

#ifdef _WIN32
	std::string programsToVsWhere = "Microsoft Visual Studio/Installer/vswhere.exe";
	std::string vswhereExecutable;

	std::string programsPath = Helpers::getEnvironment("PROGRAMFILES(X86)");
	if (programsPath.empty())
		programsPath = Helpers::getEnvironment("ProgramFiles");

	if (fs::canAccess(fs::joinPath(programsPath, programsToVsWhere).data()))
	{
		vswhereExecutable = fs::joinPath(programsPath, programsToVsWhere);
		if (vswhereExecutable.find(' ') != std::string::npos)
			vswhereExecutable = "\"" + vswhereExecutable + "\"";

		snprintf(buffer, MaxLength, "%s -latest -find VC\\Tools\\MSVC\\**\\nmake.exe", vswhereExecutable.data());
		const bool executed = Process::executeCommand(buffer, output_, Process::Echo::DISABLED);
		if (executed)
		{
			if (output_.find('\n'))
				nmakeExcutable = output_.substr(0, output_.find('\n'));

			if (fs::canAccess(nmakeExcutable.data()) == false)
				nmakeExcutable.clear();
		}
	}
#endif

	return nmakeExcutable;
}
