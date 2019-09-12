#pragma once

#include <string>

class Settings;

/// A collection of helper functions
class Helpers
{
  public:
	static const char *getEnvironment(const char *name);
	static bool setEnvironment(const char *name, const char *value);
	static bool addDirToPath(const char *directory);

	static bool checkMinVersion(unsigned int major, unsigned int minor, unsigned int patch, unsigned int minMajor, unsigned int minMinor, unsigned int minPatch);
	static bool checkMinVersion(const unsigned int version[], unsigned int minMajor, unsigned int minMinor, unsigned int minPatch);

	static void echo(const char *msg);
	static void info(const char *msg);
	static void info(const char *msg1, const char *msg2);
	static void error(const char *msg);
	static void error(const char *msg1, const char *msg2);

	static void buildDir(std::string &binaryDir, const Settings &settings);

	static const char *nCineLibrariesRepositoryUrl() { return "https://github.com/nCine/nCine-libraries.git"; }
	static const char *nCineLibrariesArtifactsRepositoryUrl() { return "https://github.com/nCine/nCine-libraries-artifacts.git"; }
	static const char *nCineLibrariesSourceDir() { return "nCine-libraries"; }
	static const char *nCineLibrariesArtifactsSourceDir() { return "nCine-libraries-artifacts"; }

	static const char *nCineRepositoryUrl() { return "https://github.com/nCine/nCine.git"; }
	static const char *nCineArtifactsRepositoryUrl() { return "https://github.com/nCine/nCine-artifacts.git"; }
	static const char *nCineDataRepositoryUrl() { return "https://github.com/nCine/nCine-data.git"; }
	static const char *nCineSourceDir() { return "nCine"; }
	static const char *nCineArtifactsSourceDir() { return "nCine-artifacts"; }
	static const char *nCineDataSourceDir() { return "nCine-data"; }

	static std::string gameRepositoryUrl(const std::string &gameName);
	static std::string gameArtifactsRepositoryUrl(const std::string &gameName);
	static std::string gameDataRepositoryUrl(const std::string &gameName);
	static std::string gameArtifactsSourceDir(const std::string &gameName);
	static std::string gameDataSourceDir(const std::string &gameName);
};
