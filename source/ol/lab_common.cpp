
// ================================================================================================
// -*- C++ -*-
// File: lab_common.cpp
// Author: Guilherme R. Lampert
// Created on: 23/09/15
// Brief: Common data structures used by the LAB reader and writer classes.
//
// This project's source code is released under the MIT License.
// - http://opensource.org/licenses/MIT
//
// ================================================================================================

#include "lab_common.hpp"
#include "filesys_utils.hpp"

namespace ol
{

// ========================================================
// fileTypeIdForFileName():
// ========================================================

void fileTypeIdForFileName(std::uint8_t id[4], const std::string & filename, const std::string & destLabFile)
{
	//
	// The exact convention used by LucasArts is unknown, so the
	// best we can do is guess based on the extension and sample
	// archives tested. Leaving the id field empty (all zeros) also
	// seems acceptable, since several entries in some archives
	// have empty type ids.
	//
	const auto ext = lowercase(filesys::getFilenameExtension(filename));
	if (ext == ".pcx")
	{
		const bool isOutlawsLAB = (destLabFile.find("outlaws.lab") != std::string::npos);
		// It seems that 'outlaws.lab' is the only archive
		// to use the 'PXCP' id for .pcx images.
		if (isOutlawsLAB)
		{
			id[0] = 'P';
			id[1] = 'X';
			id[2] = 'C';
			id[3] = 'P';
		}
		else
		{
			id[0] = 'M';
			id[1] = 'T';
			id[2] = 'X';
			id[3] = 'T';
		}
	}
	else if (ext == ".nwx")
	{
		id[0] = 'F';
		id[1] = 'X';
		id[2] = 'A';
		id[3] = 'W';
	}
	else if (ext == ".phy")
	{
		id[0] = 'S';
		id[1] = 'H';
		id[2] = 'Y';
		id[3] = 'P';
	}
	else if (ext == ".laf")
	{
		id[0] = 'T';
		id[1] = 'N';
		id[2] = 'F';
		id[3] = 'N';
	}
	else if (ext == ".rcs" || ext == ".rca")
	{
		id[0] = 'B';
		id[1] = 'P';
		id[2] = 'C';
		id[3] = 'R';
	}
	else if (ext == ".msc")
	{
		id[0] = 'B';
		id[1] = 'C';
		id[2] = 'S';
		id[3] = 'M';
	}
	else if (ext == ".wav")
	{
		id[0] = 'D';
		id[1] = 'V';
		id[2] = 'A';
		id[3] = 'W';
	}
	else if (ext == ".atx")
	{
		id[0] = 'F';
		id[1] = 'X';
		id[2] = 'T';
		id[3] = 'A';
	}
	else if (ext == ".itm")
	{
		id[0] = 'M';
		id[1] = 'E';
		id[2] = 'T';
		id[3] = 'I';
	}
	else if (ext == ".inf")
	{
		id[0] = 'F';
		id[1] = 'F';
		id[2] = 'N';
		id[3] = 'I';
	}
	else if (ext == ".3do")
	{
		id[0] = 'F';
		id[1] = 'O';
		id[2] = 'D';
		id[3] = '3';
	}
	else if (ext == ".obb" || ext == ".obt")
	{
		id[0] = 'F';
		id[1] = 'T';
		id[2] = 'B';
		id[3] = 'O';
	}
	else if (ext == ".lvb" || ext == ".lvt")
	{
		id[0] = 'F';
		id[1] = 'T';
		id[2] = 'V';
		id[3] = 'L';
	}
	else
	{
		// Unknown, try empty type-id.
		id[0] = id[1] = id[2] = id[3] = '\0';
	}
}

} // namespace ol {}
