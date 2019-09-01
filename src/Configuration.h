#pragma once

#include <memory>

namespace cpptoml {
class table;
}

/// The TOML configuration loader and saver
class Configuration
{
  public:
	Configuration();

	bool cmakeExecutable(std::string &value) const;

	bool withNinja() const;
	void setWithNinja(bool value);

	void save();

  private:
	std::shared_ptr<cpptoml::table> root_;
	std::shared_ptr<cpptoml::table> cmakeSection_;

	void retrieveSections();
};

// Meyers' Singleton
extern Configuration &config();
