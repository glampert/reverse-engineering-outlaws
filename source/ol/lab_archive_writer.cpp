
// ================================================================================================
// -*- C++ -*-
// File: lab_archive_writer.cpp
// Author: Guilherme R. Lampert
// Created on: 23/09/15
// Brief: Class that allows creating LucasArts LAB archives from loose files.
//
// This project's source code is released under the MIT License.
// - http://opensource.org/licenses/MIT
//
// ================================================================================================

#include "lab_archive_writer.hpp"
#include "lab_common.hpp"
#include "filesys_utils.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <utility>

namespace ol
{

// ========================================================
// class LabArchiveWriter:
// ========================================================

LabArchiveWriter::LabArchiveWriter(std::string destArchive, std::string sourcePath)
	: destLabFile { std::move(destArchive) }
	, srcDataPath { std::move(sourcePath)  }
{
	assert(!destLabFile.empty());
	assert(!srcDataPath.empty());

	fileList = filesys::listFilesInPath(srcDataPath);
	if (fileList.empty())
	{
		std::cerr << "Warning: Could not find any files in path \'" << srcDataPath << "\'!\n";
	}
	else // Ensure sorted.
	{
		std::sort(std::begin(fileList), std::end(fileList));
	}
}

bool LabArchiveWriter::write()
{
	if (fileList.empty())
	{
		return false;
	}

	//
	// Build the LAB archive in main memory, them flush to file when ready.
	// LABs are generally small, so this is unlikely to run out of memory
	// on any modern machine.
	//

	struct FileInfo
	{
		std::size_t nameOffset;
		std::size_t sizeInBytes;
		std::unique_ptr<std::uint8_t[]> data;

		FileInfo(std::size_t offs, std::size_t size, std::unique_ptr<std::uint8_t[]> ptr)
			: nameOffset  { offs }
			, sizeInBytes { size }
			, data        { std::move(ptr) }
		{ }
	};

	std::vector<FileInfo> srcFileInfos;
	std::uint32_t fileNameListLength = 0;

	for (const auto & fileName : fileList)
	{
		std::size_t dataSize = 0;
		auto data = filesys::loadFile(srcDataPath + fileName, &dataSize);

		if (data == nullptr)
		{
			std::cerr << "Failed to load file \'" << fileName << "\'! Won't be added to LAB archive...\n";
			// Put a nullptr in srcFileInfos anyway.
			// We need its size to match fileList's.
		}

		const std::size_t nameOffset = fileNameListLength;
		srcFileInfos.emplace_back(nameOffset, dataSize, std::move(data));

		// Size includes the null byte!
		fileNameListLength += fileName.size() + 1;
	}

	assert(srcFileInfos.size() == fileList.size());

	//
	// Once all files are loaded into memory, we can construct the archive.
	//

	filesys::createPath(destLabFile);
	FILE * fileOut = std::fopen(destLabFile.c_str(), "wb");

	if (fileOut == nullptr)
	{
		std::cerr << "Failed to open file " << destLabFile << " for writing!\n";
		return false;
	}

	const std::uint32_t fileCount = static_cast<std::uint32_t>(fileList.size());

	LabHeader labHeader;
	labHeader.id[0]              = 'L';
	labHeader.id[1]              = 'A';
	labHeader.id[2]              = 'B';
	labHeader.id[3]              = 'N';
	labHeader.unknown            = 0x10000; // This value seems to be used on all archives tested.
	labHeader.fileCount          = fileCount;
	labHeader.fileNameListLength = fileNameListLength;

	if (std::fwrite(&labHeader, sizeof(labHeader), 1, fileOut) != 1)
	{
		std::cerr << "Failed to write LAB header! " << destLabFile << ".\n";
		std::fclose(fileOut);
		return false;
	}

	std::uint32_t dataOffset = sizeof(LabHeader) +
		(fileCount * sizeof(LabFileEntry)) + fileNameListLength;

	// Write the entry headers:
	for (std::uint32_t i = 0; i < fileCount; ++i)
	{
		const auto & fileName = fileList[i];
		const auto & fileInfo = srcFileInfos[i];

		if (fileInfo.data == nullptr)
		{
			continue;
		}

		LabFileEntry labEntry;
		labEntry.dataOffset  = dataOffset;
		labEntry.nameOffset  = static_cast<std::uint32_t>(fileInfo.nameOffset);
		labEntry.sizeInBytes = static_cast<std::uint32_t>(fileInfo.sizeInBytes);
		fileTypeIdForFileName(labEntry.typeId, fileName, destLabFile);

		if (std::fwrite(&labEntry, sizeof(labEntry), 1, fileOut) != 1)
		{
			std::cerr << "Failed to write LAB entry header! " << destLabFile << ".\n";
			std::fclose(fileOut);
			return false;
		}

		dataOffset += fileInfo.sizeInBytes;
	}

	// Write the filename list (null terminated strings, including the null byte):
	for (const auto & fileName : fileList)
	{
		if (std::fwrite(fileName.c_str(), sizeof(char),
		    fileName.length() + 1, fileOut) != fileName.length() + 1)
		{
			std::cerr << "Failed to write LAB entry name! " << destLabFile << ".\n";
			std::fclose(fileOut);
			return false;
		}
	}

	// Now finally write the data for each file entry:
	for (std::uint32_t i = 0; i < fileCount; ++i)
	{
		const auto & fileInfo = srcFileInfos[i];
		if (fileInfo.data == nullptr)
		{
			continue;
		}

		if (std::fwrite(fileInfo.data.get(), sizeof(std::uint8_t),
		    fileInfo.sizeInBytes, fileOut) != fileInfo.sizeInBytes)
		{
			std::cerr << "Failed to write LAB entry data! " << destLabFile << ".\n";
			std::fclose(fileOut);
			return false;
		}
	}

	std::fclose(fileOut);
	return true;
}

} // namespace ol {}
