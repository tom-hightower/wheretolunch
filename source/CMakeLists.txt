cmake_minimum_required(VERSION 3.8)

project(WhereToLunch)

file(GLOB_RECURSE all_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.hpp" "*.h" "*.cpp")

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/lib")

add_executable(WhereToLunch ${all_files})
set_target_properties(WhereToLunch PROPERTIES LINKER_LANGUAGE CXX)
#run program in build dir when running from visual studio
set_target_properties(WhereToLunch PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

target_include_directories(WhereToLunch PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(WhereToLunch PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty")

#Load libraries
target_link_libraries(WhereToLunch libcurl_imp)
target_link_libraries(WhereToLunch jsoncpp)
