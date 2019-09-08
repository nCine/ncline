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
	bool cmakeExecutable(std::string &value) const;
	bool ninjaExecutable(std::string &value) const;

	bool withNinja() const;
	void setWithNinja(bool value);

	bool withMinGW() const;
	void setWithMinGW(bool value);

	unsigned int vsVersion() const;
	void setVsVersion(unsigned int version);

	bool branchName(std::string &value) const;
	void setBranchName(const std::string &value);

	Compiler compiler() const;
	void setCompiler(Compiler compiler);

	void print() const;
	void save();

  private:
	std::shared_ptr<cpptoml::table> root_;
	std::shared_ptr<cpptoml::table> gitSection_;
	std::shared_ptr<cpptoml::table> cmakeSection_;
	std::shared_ptr<cpptoml::table> ncineSection_;

	void retrieveSections();
};

// Meyers' Singleton
extern Configuration &config();
