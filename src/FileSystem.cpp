#include <cassert>
#include "FileSystem.h"

#ifdef _WIN32
	#define _AMD64_
	#include <windef.h>
	#include <WinBase.h>
	#include <fileapi.h>
#else
	#include <cstring>
	#include <unistd.h>
	#include <sys/stat.h>
	#include <libgen.h>
#endif

namespace {

const int MaxLength = 512;
char buffer[MaxLength];

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

std::string FileSystem::joinPath(const std::string &first, const std::string &second)
{
	const bool firstHasSeparator = first.back() == '/' || first.back() == '\\';
	const bool secondHasSeparator = second.front() == '/' || second.front() == '\\';

	if (firstHasSeparator != secondHasSeparator)
		return first + second;
	else if (firstHasSeparator == false && secondHasSeparator == false)
		return first + "/" + second;
	else
		return first + second.substr(1);
}

std::string FileSystem::dirName(const char *path)
{
	assert(path);
#ifdef _WIN32
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];

	_splitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, nullptr, 0, nullptr, 0);
	std::string directoryName(drive);
	directoryName.append(dir);
	return directoryName;
#else
	strncpy(buffer, path, MaxLength - 1);
	return std::string(dirname(buffer));
#endif
}

std::string FileSystem::baseName(const char *path)
{
	assert(path);
#ifdef _WIN32
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath_s(path, nullptr, 0, nullptr, 0, fname, _MAX_FNAME, ext, _MAX_EXT);
	std::string baseName(fname);
	baseName.append(ext);
	return baseName;
#else
	strncpy(buffer, path, MaxLength - 1);
	return std::string(basename(buffer));
#endif
}

std::string FileSystem::absolutePath(const char *relativePath)
{
	assert(relativePath);
#ifdef _WIN32
	_fullpath(buffer, relativePath, 512);
#else
	realpath(relativePath, buffer);
#endif
	return std::string(buffer);
}

std::string FileSystem::currentDir()
{
#ifdef _WIN32
	GetCurrentDirectory(MaxLength, buffer);
#else
	getcwd(buffer, MaxLength);
#endif

	return std::string(buffer);
}

bool FileSystem::isDirectory(const char *file)
{
#ifdef _WIN32
	const DWORD attrs = GetFileAttributesA(file);
	return (attrs & FILE_ATTRIBUTE_DIRECTORY);
#else
	struct stat sb;
	if (lstat(file, &sb) == -1)
	{
		perror("lstat");
		return false;
	}
	else
		return (sb.st_mode & S_IFMT) == S_IFDIR;
#endif
}

bool FileSystem::canAccess(const char *file)
{
#ifdef _WIN32
	const DWORD attrs = GetFileAttributesA(file);
	return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
	return (access(file, R_OK) == 0);
#endif
}
