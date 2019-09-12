#pragma once

#include <string>
#include <memory>

namespace cpptoml {
class table;
}

/// The TOML configuration loader and saver
class Configuration
{
  public:
	enum class Compiler
	{
		GCC,
		CLANG,

		UNSPECIFIED
	};

	Configuration();

	bool withColors() const;
	void setWithColors(bool value);

	bool gitExecutable(std::string &value) const;
	void setGitExecutable(const std::string &value);
	bool cmakeExecutable(std::string &value) const;
	void setCMakeExecutable(const std::string &value);
	bool ninjaExecutable(std::string &value) const;
	void setNinjaExecutable(const std::string &value);
	bool emcmakeExecutable(std::string &value) const;
	void setEmcmakeExecutable(const std::string &value);

	bool withEmscripten() const;
	void setWithEmscripten(bool value);

	bool withNinja() const;
	void setWithNinja(bool value);

	bool withMinGW() const;
	void setWithMinGW(bool value);

	unsigned int vsVersion() const;
	void setVsVersion(unsigned int version);

	Compiler compiler() const;
	void setCompiler(Compiler compiler);

	bool engineCMakeArguments(std::string &value) const;
	void setEngineCMakeArguments(const std::string &value);

	bool branchName(std::string &value) const;
	void setBranchName(const std::string &value);

	bool hasEngineDir() const;
	bool engineDir(std::string &value) const;
	void setEngineDir(const std::string &value);

	bool hasGameName() const;
	bool gameName(std::string &value) const;
	void setGameName(const std::string &value);

	void print() const;
	void save();

  private:
	std::shared_ptr<cpptoml::table> root_;
	std::shared_ptr<cpptoml::table> executablesSection_;
	std::shared_ptr<cpptoml::table> cmakeSection_;
	std::shared_ptr<cpptoml::table> ncineSection_;

	bool hasString(const std::shared_ptr<cpptoml::table> &section, const char *name) const;
	bool retrieveString(const std::shared_ptr<cpptoml::table> &section, const char *name, std::string &dest) const;

	void retrieveSections();
};

// Meyers' Singleton
extern Configuration &config();
