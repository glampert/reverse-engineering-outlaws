
// ================================================================================================
// -*- C++ -*-
// File: lab_archive_reader.cpp
// Author: Guilherme R. Lampert
// Created on: 23/09/15
// Brief: Simple reader and unpacker for LucasArts LAB archives.
//
// This project's source code is released under the MIT License.
// - http://opensource.org/licenses/MIT
//
// ================================================================================================

#include "lab_archive_reader.hpp"
#include "lab_common.hpp"
#include "filesys_utils.hpp"

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <memory>
#include <utility>

namespace ol
{

// ========================================================
// class LabArchiveReader:
// ========================================================

LabArchiveReader::LabArchiveReader(std::string filename)
	: labFileHandle { nullptr }
	, labFileName   { std::move(filename) }
{ }

LabArchiveReader::~LabArchiveReader()
{
	close();
}

bool LabArchiveReader::open()
{
	if (isOpen())
	{
		std::cerr << "LAB archive already open!\n";
		return false;
	}

	std::size_t fileSizeBytes = 0;
	if (!filesys::queryFileSize(labFileName, fileSizeBytes))
	{
		std::cerr << "Unable to get LAB archive file size! " << labFileName << ".\n";
		return false;
	}

	if (fileSizeBytes == 0)
	{
		std::cerr << "LAB archive size is zero! " << labFileName << ".\n";
		return false;
	}

	labFileHandle = std::fopen(labFileName.c_str(), "rb");
	if (labFileHandle == nullptr)
	{
		std::cerr << "Unable to open LAB archive file " << labFileName << " for reading!\n";
		return false;
	}

	// Get the first 4 bytes just so we can make sure this is a LAB archive,
	// before taking the trouble of loading it into memory.
	std::uint8_t id4cc[4] = {0};
	if (std::fread(id4cc, sizeof(id4cc), 1, labFileHandle) != 1)
	{
		close();
		std::cerr << "Can't read LAB id! " << labFileName << ".\n";
		return false;
	}

	// "LABN" => LucasArts BiNary, I suppose...
	if (id4cc[0] != 'L' ||
	    id4cc[1] != 'A' ||
	    id4cc[2] != 'B' ||
	    id4cc[3] != 'N')
	{
		close();
		std::cerr << "Bad LAB id! " << labFileName << ".\n";
		return false;
	}

	// Rewind back the 4CC previously read:
	std::rewind(labFileHandle);

	// Read the whole file into memory:
	labFileContents.resize(fileSizeBytes);
	if (std::fread(labFileContents.data(), sizeof(std::uint8_t),
	    fileSizeBytes, labFileHandle) != fileSizeBytes)
	{
		close();
		std::cerr << "Unable to read whole LAB archive into main memory! " << labFileName << ".\n";
		return false;
	}

	// Build the file table, etc.
	if (!loadArchiveMetadata())
	{
		close();
		return false;
	}

	return true;
}

void LabArchiveReader::close()
{
	if (labFileHandle != nullptr)
	{
		std::fclose(labFileHandle);
		labFileHandle = nullptr;
	}

	labFileContents.clear();
	labFileEntries.clear();
}

bool LabArchiveReader::isOpen() const
{
	return (labFileHandle != nullptr) && !labFileContents.empty();
}

void LabArchiveReader::listFileEntries(std::ostream & os) const
{
	os << "[[ LAB archive entries listing for \'" << labFileName << "\' ]]\n";
	if (labFileEntries.empty())
	{
		os << "(empty)\n";
	}
	else
	{
		os << "+-------------+-------------+--------------------+\n";
		os << "| dataOffset  |  sizeBytes  |  id/filename       |\n";
		os << "+-------------+-------------+--------------------+\n";
		for (const auto & entry : labFileEntries)
		{
			const char idString[] =
			{
				entry.second.typeId[0] ? entry.second.typeId[0] : '-',
				entry.second.typeId[1] ? entry.second.typeId[1] : '-',
				entry.second.typeId[2] ? entry.second.typeId[2] : '-',
				entry.second.typeId[3] ? entry.second.typeId[3] : '-',
				'\0'
			};
			os << "  "  << std::setw(11) << std::left << entry.second.dataOffset
			   << " | " << std::setw(11) << std::left << entry.second.dataSizeBytes
			   << " | " << "[" << idString << "] " << entry.first << "\n";
		}
	}
	os << "[[ listed " << labFileEntries.size() << " entries ]]\n";
}

int LabArchiveReader::extractWholeArchive(const std::string & destPath) const
{
	if (!isOpen())
	{
		std::cerr << "LAB archive not open!\n";
		return 0;
	}

	// Crate the path is necessary.
	if (!destPath.empty())
	{
		filesys::createPath(destPath);
	}

	// Data offsets for each entry are absolute from the beginning of the file.
	const auto * labDataPtr = labFileContents.data();

	int filesWritten = 0;
	std::string fullPathName;

	// Write 'em:
	for (const auto & entry : labFileEntries)
	{
		if (destPath.back() != *filesys::getPathSeparator())
		{
			fullPathName = destPath + filesys::getPathSeparator() + entry.first;
		}
		else
		{
			fullPathName = destPath + entry.first;
		}

		FILE * fileOut = std::fopen(fullPathName.c_str(), "wb");
		if (fileOut == nullptr)
		{
			std::cerr << "Failed to open file \'" << fullPathName.c_str() << "\' for writing!\n";
			continue;
		}

		const auto * myData = labDataPtr + entry.second.dataOffset;
		const auto   mySize = entry.second.dataSizeBytes;

		if (std::fwrite(myData, sizeof(*myData), mySize, fileOut) != mySize)
		{
			std::cerr << "fwrite() failed for \'" << fullPathName.c_str() << "\'!\n";
			// Count it as a success anyways...
		}

		std::fclose(fileOut);
		++filesWritten;
	}

	return filesWritten;
}

bool LabArchiveReader::loadArchiveMetadata()
{
	assert(isOpen());

	// Data starts with the LAB header:
	const auto * labHeaderPtr =
		reinterpret_cast<const LabHeader *>(labFileContents.data());

	// Followed by a list of file entry headers:
	const auto * labEntryPtr =
		reinterpret_cast<const LabFileEntry *>(labHeaderPtr + 1);

	// And after that the filename list. This is a block of null-separated ASCII strings.
	const auto * labFileNameListPtr =
		reinterpret_cast<const char *>(labEntryPtr + labHeaderPtr->fileCount);

	// Validate the id again. Not strictly needed, since we've done that already,
	// but won't harm, plus should catch potential IO errors...
	if (labHeaderPtr->id[0] != 'L' ||
	    labHeaderPtr->id[1] != 'A' ||
	    labHeaderPtr->id[2] != 'B' ||
	    labHeaderPtr->id[3] != 'N')
	{
		std::cerr << "LAB id mismatch! " << labFileName << ".\n";
		return false;
	}

	const auto fileSize  = labFileContents.size();
	const auto fileCount = labHeaderPtr->fileCount;
	const auto fileNameListLength = labHeaderPtr->fileNameListLength;

	for (std::size_t f = 0; f < fileCount; ++f, ++labEntryPtr)
	{
		const auto & entry = *labEntryPtr;

		// Watch out for corrupted data...
		if (entry.nameOffset >= fileNameListLength)
		{
			std::cerr << "Warning: LAB entry with bad name offset! Ignoring it... " << labFileName << ".\n";
			continue;
		}
		if (entry.dataOffset >= fileSize)
		{
			std::cerr << "Warning: LAB entry with bad data offset! Ignoring it... " << labFileName << ".\n";
			continue;
		}
		if ((entry.dataOffset + entry.sizeInBytes) > fileSize)
		{
			std::cerr << "Warning: LAB entry with bad data offset/size! Ignoring it... " << labFileName << ".\n";
			continue;
		}

		// Assume each filename will be terminated by one or
		// more null bytes. Should be true for a good file...
		std::string entryName  { &labFileNameListPtr[entry.nameOffset] };
		TableEntry  tableEntry { entry.dataOffset, entry.sizeInBytes, entry.typeId };
		labFileEntries.emplace(std::move(entryName), tableEntry);
	}

	return true;
}

} // namespace ol {}
