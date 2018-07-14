
// ================================================================================================
// -*- C++ -*-
// File: filesys_utils.hpp
// Author: Guilherme R. Lampert
// Created on: 23/09/15
// Brief: File System helper functions.
//
// This project's source code is released under the MIT License.
// - http://opensource.org/licenses/MIT
//
// ================================================================================================

#ifndef OL_FILESYS_UTILS_HPP
#define OL_FILESYS_UTILS_HPP

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <cstdio>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ol
{
namespace filesys
{

// Get the common path separator as a string ("/").
const char * getPathSeparator() noexcept;

// Strip the filename, retuning the extension or empty string if no extension.
std::string getFilenameExtension(const std::string & filename, bool includeDot = true);

// Get the size in byte of a file. Zero and false if the file doesn't exist.
bool queryFileSize(const std::string & filename, std::size_t & sizeInBytes);

// Create a single directory. No side effects is the dir already exists.
bool createDirectory(const std::string & dirPath);

// Create a full path of directories. No side effects if the path already exists.
bool createPath(const std::string & pathEndedWithSeparatorOrFilename);

// Get a list of all files in a directory. Can optionally ignore or add files starting
// with a dot (hidden files on Unix). Returns an empty list if an error occurs and logs to STDERR.
std::vector<std::string> listFilesInPath(const std::string & dirPath, bool allowDotFiles = false);

// Load the whole file into memory, treat as a binary file. Returns null on error.
std::unique_ptr<std::uint8_t[]> loadFile(const std::string & filename, std::size_t * sizeInBytes = nullptr);

} // namespace filesys {}
} // namespace ol {}

#endif // OL_FILESYS_UTILS_HPP
