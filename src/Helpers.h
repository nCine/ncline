#pragma once

#include <string>

class Settings;

/// A collection of helper functions
class Helpers
{
  public:
	static char *getEnv(const char *name);

	static bool checkMinVersion(unsigned int major, unsigned int minor, unsigned int patch, unsigned int minMajor, unsigned int minMinor, unsigned int minPatch);
	static bool checkMinVersion(const unsigned int version[], unsigned int minMajor, unsigned int minMinor, unsigned int minPatch);

	static void echo(const char *msg);
	static void info(const char *msg);
	static void info(const char *msg1, const char *msg2);
	static void error(const char *msg);
	static void error(const char *msg1, const char *msg2);

	static void buildDir(std::string &binaryDir, const Settings &settings);

	static const char *nCineLibrariesRepositoryUrl() { return "https://github.com/nCine/nCine-libraries.git"; }
	static const char *nCineRepositoryUrl() { return "https://github.com/nCine/nCine.git"; }
	static const char *nCineDataRepositoryUrl() { return "https://github.com/nCine/nCine-data.git"; }
	static const char *nCineLibrariesSourceDir() { return "nCine-libraries"; }
	static const char *nCineSourceDir() { return "nCine"; }
	static const char *nCineDataSourceDir() { return "nCine-data"; }
};
