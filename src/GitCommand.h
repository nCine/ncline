#pragma once

#include <string>

class GitCommand
{
  public:
	explicit GitCommand(const char *executable);

	bool clone(const char *repository);
	bool clone(const char *repository, const char *branch);
	bool checkRepositoryVersion(const char *repository, std::string &version);

	inline bool found() const { return found_; }
	inline const std::string &executable() const { return executable_; }
	inline const std::string &output() const { return output_; }

  private:
	bool found_;
	std::string executable_;
	int version_[3];

	std::string output_;
};
