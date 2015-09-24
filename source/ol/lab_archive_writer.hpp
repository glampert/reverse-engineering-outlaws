
// ================================================================================================
// -*- C++ -*-
// File: lab_archive_writer.hpp
// Author: Guilherme R. Lampert
// Created on: 23/09/15
// Brief: Class that allows creating LucasArts LAB archives from loose files.
//
// This project's source code is released under the MIT License.
// - http://opensource.org/licenses/MIT
//
// ================================================================================================

#ifndef OL_LAB_ARCHIVE_WRITER_HPP
#define OL_LAB_ARCHIVE_WRITER_HPP

#include <string>
#include <vector>

namespace ol
{

// ========================================================
// class LabArchiveWriter:
// ========================================================

class LabArchiveWriter final
{
public:

	// Disable copy and assignment.
	LabArchiveWriter(const LabArchiveWriter &) = delete;
	LabArchiveWriter & operator = (const LabArchiveWriter &) = delete;

	// Construct with the name of the output LAB archive
	// and the path where to look for files to pack.
	LabArchiveWriter(std::string destArchive, std::string sourcePath);

	// Writes the LAB archive to its destination file.
	// Files from the source path are only opened now.
	bool write();

private:

	std::vector<std::string> fileList;
	const std::string destLabFile;
	const std::string srcDataPath;
};

} // namespace ol {}

#endif // OL_LAB_ARCHIVE_WRITER_HPP
