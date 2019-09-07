#pragma once

#include <string>

class GitCommand
{
  public:
	GitCommand();

	bool clone(const char *repositoryUrl);
	bool clone(const char *repositoryUrl, const char *branch);
	bool checkRepositoryVersion(const char *repositoryDir, std::string &version);

	inline bool found() const { return found_; }
	inline const std::string &executable() const { return executable_; }
	inline const std::string &output() const { return output_; }

  private:
	bool found_;
	std::string executable_;
	unsigned int version_[3];

	std::string output_;

	bool checkPredefinedLocations();
};
