cmake_minimum_required (VERSION 2.8)

project(bigiottest)

aux_source_directory (. BIGIOT_TEST_SOURCE)
add_executable (bigiottest ${BIGIOT_TEST_SOURCE})

include_directories (../include)
target_link_libraries (bigiottest bigiot curl pthread jpeg)
