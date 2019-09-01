#include "FileSystem.h"

#ifndef __WIN32
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
	// TODO: on Windows use GetFileAttributes

	struct stat sb;
	if (lstat(file, &sb) == -1)
	{
		perror("lstat");
		return false;
	}
	else
		return (sb.st_mode & S_IFMT) == S_IFDIR;
}
