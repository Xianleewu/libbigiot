cmake_minimum_required (VERSION 2.8)

project (bigiot)

include_directories (.)
aux_source_directory (./source BIGIOT_SOURCE)
add_executable (bigiot ${BIGIOT_SOURCE})

target_link_libraries (bigiot pthread)
