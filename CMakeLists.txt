cmake_minimum_required(VERSION 3.0)
project (csmapfx CXX C)

if (WIN32)
if (CMAKE_GENERATOR_PLATFORM EQUAL x86)
set (VCPKG_TARGET_TRIPLET x86-windows)
elseif (CMAKE_GENERATOR_PLATFORM EQUAL x64)
set (VCPKG_TARGET_TRIPLET x64-windows)
endif()
endif (WIN32)

include (CheckIncludeFile)
check_include_file(unistd.h UNISTD_H_FOUND)
if (UNISTD_H_FOUND)
  add_compile_definitions(HAVE_UNISTD_H)
endif (UNISTD_H_FOUND)

include (CheckFunctionExists)
check_function_exists(mkstemp MKSTEMP_FOUND)
if (MKSTEMP_FOUND)
  add_compile_definitions(HAVE_MKSTEMP)
endif (MKSTEMP_FOUND)

set (CPARSE_SOURCES
  cparse/shunting-yard.cpp
  cparse/packToken.cpp
  cparse/functions.cpp
  cparse/containers.cpp
  cparse/builtin-features.cpp
)

set (SOURCES
  ${CPARSE_SOURCES}
	calc.cpp
	commands.cpp
	csmap.cpp
	datablock.cpp
	datafile.cpp
	exportdlg.cpp
	FXFileDialogEx.cpp
	fxhelper.cpp
	FXMenuSeparatorEx.cpp
	FXSplitterEx.cpp
	imageio.cpp
	infodlg.cpp
	main.cpp
	mapcanvas.cpp
	map.cpp
	messages.cpp
	prefsdlg.cpp
	regioninfos.cpp
	regionlist.cpp
	reportinfo.cpp
	searchdlg.cpp
	statistics.cpp
	statsinfos.cpp
	symbols.cpp
	tradeinfos.cpp
	unitlist.cpp
)

if (WIN32)
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
else (WIN32)
add_executable(${PROJECT_NAME} ${SOURCES})
endif (WIN32)

find_package(CURL CONFIG REQUIRED)
find_package(PNG REQUIRED)
find_package(PhysFS 2.1 REQUIRED)

find_library(LEGACY_STDIO_LIBRARY NAMES legacy_stdio_definitions)
if (LEGACY_STDIO_LIBRARY)
target_link_libraries(${PROJECT_NAME} ${LEGACY_STDIO_LIBRARY})
endif ()

if (WIN32)
set(FOX_ROOT $ENV{FOX_ROOT})
if (NOT FOX_ROOT)
set (FOX_ROOT "c:/homebrew/installed/x86-windows/fox-1.6.57")
endif (NOT FOX_ROOT)
find_path(FOX_INCLUDE_DIRS "fxkeys.h" PATHS "${FOX_ROOT}/include" REQUIRED)
find_library(FOX_LIBRARY NAMES FOX-1.6 PATHS "${FOX_ROOT}/lib" REQUIRED)
set (FOX_LIBRARIES ${FOX_LIBRARY})
add_compile_definitions(
FLOAT_MATH_FUNCTIONS)
else (WIN32)
 include (FindPkgConfig)
 if (PKG_CONFIG_EXECUTABLE)
  pkg_search_module(FOX REQUIRED fox>=1.6)
 endif (PKG_CONFIG_EXECUTABLE)
endif (WIN32)

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 11)
target_link_libraries(${PROJECT_NAME}
  ${PHYSFS_LIBRARY}
  ${PNG_LIBRARIES}
  ${CURL_LIBRARIES}
  ${FOX_LIBRARIES}
  ${CMAKE_DL_LIBS} 
)
if (FOX_LIBDIR)
  target_link_libraries(${PROJECT_NAME} -L${FOX_LIBDIR})
endif(FOX_LIBDIR)
if (FOX_LIBRARY_DIRS)
  target_link_libraries(${PROJECT_NAME} -L${FOX_LIBRARY_DIRS})
endif(FOX_LIBRARY_DIRS)
include_directories(
  ${PHYSFS_INCLUDE_DIR}
	${PNG_INCLUDE_DIRS}
  ${CURL_INCLUDE_DIRS}
	${FOX_INCLUDE_DIRS}
)

set(INSTALL_BIN_DIR "bin" CACHE STRING "Install executable in this directory")
set(INSTALL_DATA_DIR "share/csmapfx" CACHE STRING "Install data to this directory")
set(INSTALL_APPLICATIONS_DIR "share/applications" CACHE STRING "Install the .desktop file to this directory")


install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_BIN_DIR})
install(FILES csmapfx.xpm DESTINATION ${INSTALL_DATA_DIR})
install(FILES csmapfx.desktop DESTINATION ${INSTALL_APPLICATIONS_DIR})
