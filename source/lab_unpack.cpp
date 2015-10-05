
// ================================================================================================
// -*- C++ -*-
// File: lab_unpack.cpp
// Author: Guilherme R. Lampert
// Created on: 05/10/15
// Brief: Simple command line tool to unpack a LucasArts LAB archive.
// ================================================================================================

#include "ol/filesys_utils.hpp"
#include "ol/lab_archive_reader.hpp"

#include <string>
#include <iostream>
#include <cstdlib>

static void printHelpText(const char * progName)
{
	std::cout
		<< "\n"
		<< "Usage:\n"
		<< "$ " << progName << " <input_lab> <output_dir> [--verbose | -v]\n"
		<< "  Unpacks each file in the given LAB archive to the provided path.\n"
		<< "  Creates directories as needed. Existing files are overwritten.\n"
		<< "  If the --verbose|-v flag is provided, prints a list of files and other running stats to STDOUT.\n"
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

	// From here on we need an input filename and an output path.
	if (argc < 3)
	{
		std::cerr << "Not enough arguments!\n";
		printHelpText(argv[0]);
		return EXIT_FAILURE;
	}

	bool verbose = false;
	const std::string labFileName = argv[1];

	// Make sure the path ends with a '/' or backslash.
	std::string outputDir = argv[2];
	if (outputDir.back() != ol::filesys::getPathSeparator()[0])
	{
		outputDir += ol::filesys::getPathSeparator();
	}

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
		std::cout << "Input  file: \"" << labFileName << "\"\n";
		std::cout << "Output path: \"" << outputDir   << "\"\n";
	}

	ol::LabArchiveReader labReader { labFileName };
	if (!labReader.open())
	{
		std::cerr << "Unable to open the specified LAB archive!\n";
		return EXIT_FAILURE;
	}

	// Optional file list dump:
	if (verbose)
	{
		labReader.listFileEntries(std::cout);
	}

	// Extract:
	if (verbose) { std::cout << "Extracting files...\n"; }
	labReader.extractWholeArchive(outputDir);
	if (verbose) { std::cout << "Done!\n"; }

	return EXIT_SUCCESS;
}
