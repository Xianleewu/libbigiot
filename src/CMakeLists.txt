cmake_minimum_required (VERSION 2.8)

project(bigiot)

aux_source_directory (. LIBBIGIOT_SOURCE)
add_library (bigiot ${LIBBIGIOT_SOURCE})

include_directories (../include)
target_link_libraries (bigiot pthread jpeg)
