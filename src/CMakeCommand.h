#pragma once

#include <string>

class CMakeCommand
{
  public:
	CMakeCommand();

	static bool generatorIsMultiConfig();
	static bool generatorIsVisualStudio() { return generatorIsMultiConfig(); }

	const char *removeFile();
	const char *removeDir();
	bool toolsMode(const char *command);

	bool configure(const char *srcDir, const char *binDir, const char *generator, const char *platform, const char *arguments);
	bool configure(const char *srcDir, const char *binDir, const char *arguments);
	bool configure(const char *srcDir, const char *binDir);

	bool build(const char *buildDir, const char *config, const char *target);
	inline bool build(const char *buildDir) { return build(buildDir, nullptr, nullptr); }
	inline bool buildConfig(const char *buildDir, const char *config) { return build(buildDir, config, nullptr); }
	inline bool buildTarget(const char *buildDir, const char *target) { return build(buildDir, nullptr, target); }

	inline bool found() const { return found_; }
	inline const std::string &executable() const { return executable_; }
	bool isUpdated() const;

	inline bool ninjaFound() const { return ninjaFound_; }
	inline const std::string &ninjaExecutable() const { return ninjaExecutable_; }

	inline const std::string &output() const { return output_; }

	void addAndroidNdkDirToPath();
	void addNMakeDirToPath();
	void addAndroidSdkDirToPath();
	void addGradleDirToPath();
	void addDoxygenDirToPath();

  private:
	bool found_;
	std::string executable_;
	unsigned int version_[3];

	bool ninjaFound_;
	std::string ninjaExecutable_;
	unsigned int ninjaVersion_[3];
	std::string emcmakeExecutable_;

	std::string output_;

	const char *generator() const;
	const char *platform() const;

	bool checkPredefinedLocations();
	void findNinja();
	std::string findNMake();
};
