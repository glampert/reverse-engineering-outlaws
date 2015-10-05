
// ================================================================================================
// -*- C++ -*-
// File: lab_pack.cpp
// Author: Guilherme R. Lampert
// Created on: 05/10/15
// Brief: Simple command line tool to pack files into LucasArts LAB archives.
// ================================================================================================

#include "ol/filesys_utils.hpp"
#include "ol/lab_archive_writer.hpp"

#include <string>
#include <iostream>
#include <cstdlib>

static void printHelpText(const char * progName)
{
	std::cout
		<< "\n"
		<< "Usage:\n"
		<< "$ " << progName << " <input_dir> <output_lab> [--verbose | -v]\n"
		<< "  Packs each file in the provided directory path into a single LAB archive.\n"
		<< "  If the --verbose|-v flag is provided, prints miscellaneous running stats to STDOUT.\n"
		<< "\n"
		<< "Usage:\n"
		<< "$ " << progName << " --help | -h\n"
		<< "  Prints this help text.\n"
		<< "\n";
}

int main(int argc, const char * argv[])
{
	// At least the program name and source file/help-flag.
	if (argc < 2)
	{
		std::cerr << "Not enough arguments!\n";
		printHelpText(argv[0]);
		return EXIT_FAILURE;
	}

	// Printing help is not treated as an error.
	if (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0)
	{
		printHelpText(argv[0]);
		return EXIT_SUCCESS;
	}

	// From here on we need an input path and an output file.
	if (argc < 3)
	{
		std::cerr << "Not enough arguments!\n";
		printHelpText(argv[0]);
		return EXIT_FAILURE;
	}

	// Make sure the path ends with a '/' or backslash.
	std::string inputDir = argv[1];
	if (inputDir.back() != ol::filesys::getPathSeparator()[0])
	{
		inputDir += ol::filesys::getPathSeparator();
	}

	const std::string outputLab = argv[2];
	bool verbose = false;

	// Possible verbose flag, ignore if anything else.
	if (argc >= 4)
	{
		if (std::strcmp(argv[3], "-v") == 0 || std::strcmp(argv[3], "--verbose") == 0)
		{
			verbose = true;
		}
	}

	if (verbose)
	{
		std::cout << "Input path:     \"" << inputDir  << "\"\n";
		std::cout << "Output archive: \"" << outputLab << "\"\n";
		std::cout << "Preparing to write LAB archive...\n";
	}

	ol::LabArchiveWriter labWriter { outputLab, inputDir };
	if (!labWriter.write())
	{
		std::cerr << "Failed to write specified LAB archive!\n";
		return EXIT_FAILURE;
	}

	if (verbose)
	{
		std::cout << "LAB archive successfully created!\n";
	}
	return EXIT_SUCCESS;
}
