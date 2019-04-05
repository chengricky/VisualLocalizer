# CMakeList.txt: VisualLocalizer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# TODO: 如有需要，请添加测试并安装目标。
SET(OpenCV_DIR "E:/Dependencies/opencv401/build")
find_package(OpenCV)
SET(OrTools_DIR "E:/Dependencies/or-tools_VisualStudio2017-64bit_v6.7.4973")
include_directories(${OpenCV_INCLUDE_DIRS} ${OrTools_DIR}/include)

FILE(GLOB_RECURSE CURRENT_INCLUDE *.h graph/*.h )
FILE(GLOB_RECURSE CURRENT_SOURCES *.cpp graph/*.cpp)     #aux_source_directory(src CURRENT_SOURCES)

# 将源代码添加到此项目的可执行文件。
add_executable (VisualLocalizer ${CURRENT_SOURCES} ${CURRENT_INCLUDE})

target_link_libraries(VisualLocalizer 
${OpenCV_LIBS} 
${OrTools_DIR}/lib/ortools.lib
)