
#include <iostream>

#include "ol/filesys_utils.hpp"
#include "ol/lab_archive_reader.hpp"
#include "ol/lab_archive_writer.hpp"

int main(/* int argc, const char * argv[] */)
{
	ol::LabArchiveReader reader { "outlaws.lab" };
	ol::LabArchiveWriter writer { "my_outlaws.lab", "dump/outlaws/" };

	std::cout << "Nothing going on here right now. Come back later...\n";
}
