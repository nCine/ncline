#pragma once

#include <string>

class FileSystem
{
  public:
	static std::string joinPath(const std::string &first, const std::string &second);
	static std::string dirName(const char *path);
	static std::string baseName(const char *path);
	static std::string absolutePath(const char *relativePath);
	static std::string currentDir();
	static bool isDirectory(const char *file);
	static bool canAccess(const char *file);
};
