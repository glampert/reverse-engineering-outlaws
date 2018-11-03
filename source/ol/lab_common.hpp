
// ================================================================================================
// -*- C++ -*-
// File: lab_common.hpp
// Author: Guilherme R. Lampert
// Created on: 23/09/15
// Brief: Common data structures used by the LAB reader and writer classes.
//
// This project's source code is released under the MIT License.
// - http://opensource.org/licenses/MIT
//
// ================================================================================================

#ifndef OL_LAB_COMMON_HPP
#define OL_LAB_COMMON_HPP

#include <cstdint>
#include <cctype>
#include <string>
#include <functional>
#if defined(_WIN32)
#include <algorithm>
#endif

namespace ol
{

//
// Partial file format describe here:
//   http://wiki.xentax.com/index.php?title=Lucus_Arts_LAB
//
// Layout presented in the link is incorrect, at least
// when compared to the sample archives tested by me!
//

#pragma pack(push, 1)

struct LabHeader
{
	std::uint8_t  id[4];              // Always 'LABN'.
	std::uint32_t unknown;            // Apparently always 0x10000 for Outlaws.
	std::uint32_t fileCount;          // File entry count.
	std::uint32_t fileNameListLength; // Length including null bytes of the filename list/string.
};

struct LabFileEntry
{
	std::uint32_t nameOffset;         // Offset in the name string.
	std::uint32_t dataOffset;         // Offset in the archive file.
	std::uint32_t sizeInBytes;        // Size in bytes of this entry.
	std::uint8_t  typeId[4];          // All zeros or a 4CC related to the filename extension.
};

#pragma pack(pop)

// ========================================================

inline std::string lowercase(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

inline std::string uppercase(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str;
}

void fileTypeIdForFileName(std::uint8_t id[4], const std::string & filename, const std::string & destLabFile);

} // namespace ol {}

#endif // OL_LAB_COMMON_HPP
