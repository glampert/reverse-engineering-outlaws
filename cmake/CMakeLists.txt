cmake_minimum_required(VERSION 3.0.2)

project(OLTools)

set(CMAKE_MODULE_PATH "${OLTools_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 14)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(src_root ${OLTools_SOURCE_DIR}/../source)

add_library(OL STATIC
	${src_root}/ol/filesys_utils.cpp
	${src_root}/ol/filesys_utils.hpp
	${src_root}/ol/lab_archive_reader.cpp
	${src_root}/ol/lab_archive_reader.hpp
	${src_root}/ol/lab_archive_writer.cpp
	${src_root}/ol/lab_archive_writer.hpp
	${src_root}/ol/lab_common.cpp
	${src_root}/ol/lab_common.hpp)

set(lab_libraries
	OL)

add_executable(lab_unpack
	${src_root}/lab_unpack.cpp)

add_executable(lab_pack
	${src_root}/lab_pack.cpp)

target_link_libraries(lab_unpack
	${lab_libraries})

target_link_libraries(lab_pack
	${lab_libraries})

target_include_directories(lab_pack PRIVATE ${src_root}/ol)
target_include_directories(lab_unpack PRIVATE ${src_root}/ol)