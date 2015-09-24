
// ================================================================================================
// -*- C++ -*-
// File: lab_archive_reader.hpp
// Author: Guilherme R. Lampert
// Created on: 23/09/15
// Brief: Simple reader and unpacker for LucasArts LAB archives.
//
// This project's source code is released under the MIT License.
// - http://opensource.org/licenses/MIT
//
// ================================================================================================

#ifndef OL_LAB_ARCHIVE_READER_HPP
#define OL_LAB_ARCHIVE_READER_HPP

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

namespace ol
{

// ========================================================
// class LabArchiveReader:
// ========================================================

class LabArchiveReader final
{
public:

	// Disable copy and assignment.
	LabArchiveReader(const LabArchiveReader &) = delete;
	LabArchiveReader & operator = (const LabArchiveReader &) = delete;

	// Construct with the name of the file that will be
	// opened for reading by the open() method.
	explicit LabArchiveReader(std::string filename);

	// Open the archive using the path/name
	// provided on construction.
	bool open();

	// Manually closes the archive file.
	// Done automatically by the destructor.
	void close();

	// Test if the archive was successfully opened.
	bool isOpen() const;

	// Print a list of all entries present in the LAB archive.
	void listFileEntries(std::ostream & os = std::cout) const;

	// Extracts all LAB archive files to the destination path, creating
	// directories as needed and overwriting existing files. Returns the
	// number of files successfully extracted. Errors logged to STDERR.
	int extractWholeArchive(const std::string & destPath) const;

	// Destructor automatically closes the archive.
	~LabArchiveReader();

private:

	bool loadArchiveMetadata();

	struct TableEntry
	{
		std::uint32_t dataOffset;
		std::uint32_t dataSizeBytes;
		char          typeId[4]; // 4CC from the entry header, for displaying.

		TableEntry(std::uint32_t offs, std::uint32_t size, const std::uint8_t id[4])
			: dataOffset    { offs }
			, dataSizeBytes { size }
		{
			typeId[0] = static_cast<char>(id[0]);
			typeId[1] = static_cast<char>(id[1]);
			typeId[2] = static_cast<char>(id[2]);
			typeId[3] = static_cast<char>(id[3]);
		}
	};

	using FileTable   = std::unordered_map<std::string, TableEntry>;
	using ByteVector  = std::vector<std::uint8_t>;

	FILE *            labFileHandle;
	ByteVector        labFileContents;
	FileTable         labFileEntries;
	const std::string labFileName;
};

} // namespace ol {}

#endif // OL_LAB_ARCHIVE_READER_HPP
