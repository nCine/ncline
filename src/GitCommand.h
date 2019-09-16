#pragma once

#include <string>

class GitCommand
{
  public:
	GitCommand();

	bool customCommand(const char *repositoryDir, const char *command);

	bool clone(const char *repositoryUrl, const char *branch, unsigned int depth, bool noCheckout);
	inline bool clone(const char *repositoryUrl, const char *branch, unsigned int depth) { return clone(repositoryUrl, branch, depth, false); }
	inline bool clone(const char *repositoryUrl, const char *branch) { return clone(repositoryUrl, branch, 0); }
	bool clone(const char *repositoryUrl);
	bool checkout(const char *repositoryDir, const char *branch, const char *workTreeDir);
	inline bool checkout(const char *repositoryDir, const char *branch) { return checkout(repositoryDir, branch, repositoryDir); }
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
