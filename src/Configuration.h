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

	void print() const;
	void save();

  private:
	std::shared_ptr<cpptoml::table> root_;
	std::shared_ptr<cpptoml::table> gitSection_;
	std::shared_ptr<cpptoml::table> cmakeSection_;

	void retrieveSections();
};

// Meyers' Singleton
extern Configuration &config();
