#include "stdafx.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#
#	include <shlobj.h>  // SHGetFolderPath
#	include <shlwapi.h> // PathAppend
#	include <direct.h>  // _tchdir and _tgetcwd
#
#	define stat _stat
#
#   define PATH_SEPARATOR ( '\\' )
#
#   ifndef W_OK
#       define W_OK _S_IREAD
#   endif
#
#   ifndef R_OK
#       define R_OK _S_IWRITE
#   endif
#
#else
#
#	include <pwd.h>
#
#   define PATH_SEPARATOR ( '/' )
#
#endif

void createDirectory(const FileName &path)
{
    int result = -1;

#ifdef _WIN32
	result = _mkdir(path.c_str());
#else
    result = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif

	if(result < 0)
	{
		if(errno != EEXIST) // directory exists is OK
		{
			perror("mkdir() failed");
			ERR(string("mkdir() failed: ") + strerror(errno));
		}
	}
}

bool setWorkingDirectory(const FileName &path)
{
    int result = -1;

#ifdef _WIN32
    result = _chdir(path.c_str());
#else
    result = chdir(path.c_str());
#endif

    return result >= 0;
}

FileName getWorkingDirectory()
{
	char *pszWorkingDirectory = 0;

#ifdef _WIN32
	pszWorkingDirectory = _getcwd(0,0);
#else
	pszWorkingDirectory = getcwd(0,0);
#endif

	string workingDirectory = pszWorkingDirectory;

	free(pszWorkingDirectory);

	return workingDirectory;
}

FileName getUserSettingsDirectory()
{
	FileName finalPath("./");

#ifdef _WIN32
	char homeDir[MAX_PATH] = {0};


	if (SUCCEEDED(SHGetFolderPath(NULL,
	                              CSIDL_APPDATA | CSIDL_FLAG_CREATE,
	                              NULL,
	                              0,
	                              homeDir)))
	{
		finalPath = FileName::append(homeDir, PROJECT_NAME);
	}

#else

	/*
	Expects game settings to be located in ~/.{PROJECT_NAME}
	(This is a hidden directory within the user's home directory)
	TODO: Follow the standard universal file hierarchy better
	*/

    struct passwd *pwd = getpwuid(getuid());

    if(pwd == 0)
    {
        perror("Failed to retrieve user information");
        finalPath = string("./.") + PROJECT_NAME;
    }
    else
    {
		finalPath = FileName::append(pwd->pw_dir, string(".") + PROJECT_NAME);
    }
#endif

	// Ensure that the directory exists
	createDirectory(finalPath);

	return finalPath;
}

FileName getApplicationDirectory()
{
#ifdef _WIN32

	char pathBuffer[_MAX_PATH];

	if(GetModuleFileName(GetModuleHandle(NULL), pathBuffer, _MAX_PATH-1) != 0)
	{
		// Strip off the filename and extension
		size_t i;
		for(i = strlen(pathBuffer) - 1; i > 0; --i)
		{
			if(pathBuffer[i] == '\\')
			{
				pathBuffer[i]=0;
				break;
			}
		}

		if(i != 0)
		{
			return pathBuffer;
		}
	}

	return ".\\";

#else

	/*
	Expects game data to be located in ~/{PROJECT_NAME}
	(This is a directory within the user's home directory)
	TODO: Follow the standard universal file hierarchy better
	*/

	string finalPath = "./";

	struct passwd *pwd = getpwuid(getuid());

	if(pwd == 0)
	{
		perror("Failed to retrieve user information");
		finalPath = string("./") + PROJECT_NAME;
	}
	else
	{
		finalPath = FileName::append(pwd->pw_dir, PROJECT_NAME);
	}

	return finalPath;

#endif
}

bool isFileOnDisk(const FileName &fileName)
{
	struct stat info;

	// if we can stat the file, then it does exist
	return (stat(fileName.c_str(), &info) == 0);
}
