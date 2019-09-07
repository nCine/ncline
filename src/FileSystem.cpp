#include "FileSystem.h"

#ifdef _WIN32
	#define _AMD64_
	#include <fileapi.h>
#else
	#include <unistd.h>
	#include <sys/stat.h>
#endif

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
