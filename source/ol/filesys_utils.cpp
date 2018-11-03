
// ================================================================================================
// -*- C++ -*-
// File: filesys_utils.cpp
// Author: Guilherme R. Lampert
// Created on: 23/09/15
// Brief: File System helper functions.
//
// This project's source code is released under the MIT License.
// - http://opensource.org/licenses/MIT
//
// ================================================================================================

#include "filesys_utils.hpp"

// STD C:
#include <errno.h>
#include <cassert>
#include <cstring>
#include <iostream>

// POSIX includes:
#include <sys/types.h>
#include <sys/stat.h>
#if defined(_WIN32)
#include <direct.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif
namespace ol
{
namespace filesys
{

// ========================================================
// getPathSeparator():
// ========================================================

const char * getPathSeparator() noexcept
{
	return "/";
}

// ========================================================
// getFilenameExtension():
// ========================================================

std::string getFilenameExtension(const std::string & filename, const bool includeDot)
{
	std::string extension;
	const auto lastDot = filename.find_last_of('.');
	if (lastDot != std::string::npos)
	{
		const long extensionChars = filename.length() - lastDot;
		if (extensionChars > 0)
		{
			extension.assign(filename, includeDot ? lastDot : (lastDot + 1), extensionChars);
		}
	}
	return extension;
}

// ========================================================
// queryFileSize():
// ========================================================

#if defined(_WIN32)
bool queryFileSize(const std::string & filename, std::size_t & sizeInBytes)
{
    assert(!filename.empty());

    struct stat statBuf = {};
    DWORD fileAttr = GetFileAttributesA(filename.c_str());
    if (stat(filename.c_str(), &statBuf) == 0 && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY))
    {
        sizeInBytes = static_cast<std::size_t>(statBuf.st_size);
        return true;
    }

    sizeInBytes = 0;
    return false;
}
#else
bool queryFileSize(const std::string & filename, std::size_t & sizeInBytes)
{
	assert(!filename.empty());

	struct stat statBuf = {};
	if (stat(filename.c_str(), &statBuf) == 0 && S_ISREG(statBuf.st_mode))
	{
		sizeInBytes = static_cast<std::size_t>(statBuf.st_size);
		return true;
	}

	sizeInBytes = 0;
	return false;
}
#endif

// ========================================================
// createDirectory():
// ========================================================
#if defined(_WIN32)
bool createDirectory(const std::string & dirPath)
{
    assert(!dirPath.empty());

    DWORD fileAttr = GetFileAttributesA(dirPath.c_str());
    // Directory already exists
    if (fileAttr & FILE_ATTRIBUTE_DIRECTORY)
        return false;

    // Create the folder
    if (_mkdir(dirPath.c_str()) != 0)
        return false;

    return true;
}
#else
bool createDirectory(const std::string & dirPath)
{
	assert(!dirPath.empty());

	struct stat dirStat = {};
	if (stat(dirPath.c_str(), &dirStat) != 0)
	{
		if (mkdir(dirPath.c_str(), 0777) != 0)
		{
			return false;
		}
	}
	else // Path already exists:
	{
		if (!S_ISDIR(dirStat.st_mode))
		{
			// Looks like there is a file with the same name
			// as the directory we are trying to create!
			return false;
		}
	}

	return true;
}
#endif

// ========================================================
// createPath():
// ========================================================

bool createPath(const std::string & pathEndedWithSeparatorOrFilename)
{
	char dirPath[1024];

	assert(!pathEndedWithSeparatorOrFilename.empty());
	assert(pathEndedWithSeparatorOrFilename.length() < sizeof(dirPath) && "Pathname too long!");

	std::strncpy(dirPath, pathEndedWithSeparatorOrFilename.c_str(), sizeof(dirPath) - 1);
	dirPath[sizeof(dirPath) - 1] = '\0';

	char * pPath = dirPath;
	while (*pPath != '\0')
	{
		// Works for both Win and Unix without the need for extra tweaks.
		if (*pPath == '/' || *pPath == '\\')
		{
			*pPath = '\0';
			if (!createDirectory(dirPath))
			{
				return false;
			}
			*pPath = *getPathSeparator();
		}
		++pPath;
	}

	return true;
}

// ========================================================
// listFilesInPath():
// ========================================================

#if defined(_WIN32)
std::vector<std::string> listFilesInPath(const std::string & dirPath, const bool allowDotFiles)
{
    assert(!dirPath.empty());
    std::vector<std::string> fileList;

    WIN32_FIND_DATA fileData;
    HANDLE hFile = FindFirstFileA((dirPath + "\\*").c_str(), &fileData);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        do {
            fileList.push_back(fileData.cFileName);
        } while (FindNextFile(hFile, &fileData));
    }

    return fileList;
}
#else
std::vector<std::string> listFilesInPath(const std::string & dirPath, const bool allowDotFiles)
{
	assert(!dirPath.empty());
	std::vector<std::string> fileList;

	errno = 0;
	DIR * dirPtr = opendir(dirPath.c_str());
	if (dirPtr == nullptr)
	{
		std::cerr << "opendir() failed: " << std::strerror(errno) << ".\n";
		return fileList;
	}

	dirent * dEntry = readdir(dirPtr);
	while (dEntry != nullptr)
	{
		if (allowDotFiles) // Don't care about hidden files, ".", "..", etc.
		{
			fileList.emplace_back(dEntry->d_name);
		}
		else
		{
			if (dEntry->d_name[0] != '.')
			{
				fileList.emplace_back(dEntry->d_name);
			}
		}

		dEntry = readdir(dirPtr);
	}

	closedir(dirPtr);
	return fileList;
}
#endif
// ========================================================
// loadFile():
// ========================================================

std::unique_ptr<std::uint8_t[]> loadFile(const std::string & filename, std::size_t * sizeInBytes)
{
	std::size_t fileLength = 0;
	if (!queryFileSize(filename, fileLength))
	{
		return nullptr;
	}

	if (fileLength == 0)
	{
		if (sizeInBytes != nullptr) { *sizeInBytes = 0; }
		return nullptr;
	}

	FILE * fileIn = std::fopen(filename.c_str(), "rb");
	if (fileIn == nullptr)
	{
		if (sizeInBytes != nullptr) { *sizeInBytes = 0; }
		return nullptr;
	}

	auto data = std::make_unique<std::uint8_t[]>(fileLength);
	if (std::fread(data.get(), sizeof(std::uint8_t), fileLength, fileIn) != fileLength)
	{
		std::cerr << "Partial fread() in loadFile()!\n";
		if (sizeInBytes != nullptr) { *sizeInBytes = 0; }
		std::fclose(fileIn);
		return nullptr;
	}

	// Success.
	if (sizeInBytes != nullptr)
	{
		*sizeInBytes = fileLength;
	}
	std::fclose(fileIn);
	return data;
}

} // namespace filesys {}
} // namespace ol {}
