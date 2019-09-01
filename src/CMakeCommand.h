#pragma once

#include <string>

class CMakeCommand
{
  public:
	explicit CMakeCommand(const char *executable);

	bool configure(const char *srcDir, const char *binDir, const char *arguments);
	bool configure(const char *srcDir, const char *binDir);

	bool build(const char *buildDir, const char *config, const char *target);
	inline bool build(const char *buildDir) { return build(buildDir, nullptr, nullptr); }
	inline bool buildConfig(const char *buildDir, const char *config) { return build(buildDir, config, nullptr); }
	inline bool buildTarget(const char *buildDir, const char *target) { return build(buildDir, nullptr, target); }

	inline bool found() const { return found_; }
	inline const std::string &executable() const { return executable_; }
	inline const std::string &output() const { return output_; }

  private:
	const char *generator();
	const char *platform();
	void findNinja();

	bool found_;
	bool ninjaFound_;
	std::string executable_;
	int version_[3];
	int ninjaVersion_[3];

	std::string output_;
};
