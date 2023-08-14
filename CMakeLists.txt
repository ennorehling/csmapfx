cmake_minimum_required(VERSION 3.5)

project (csmapfx CXX C)

if (WIN32)
## set to FALSE to prefer the WinAPI PNG encoder:
set(USE_LIBPNG FALSE)
add_compile_definitions(-DUNICODE)
add_compile_definitions(WITH_MAP_EXPORT)
set(PLATFORM_SOURCES win32.cpp)
else (WIN32)
set(USE_LIBPNG TRUE)
find_package(CURL)
find_package(PhysFS 2.1)
set(PLATFORM_SOURCES upload.cpp)
endif (WIN32)

if (USE_LIBPNG)
find_package (PNG)
if (PNG_FOUND)
add_compile_definitions(WITH_MAP_EXPORT)
set(PLATFORM_SOURCES ${PLATFORM_SOURCES} imageio.cpp)
else (PNG_FOUND)
set (USE_LIBPNG FALSE)
message(STATUS "LIBPNG ${PNG_LIBRARY}")
endif (PNG_FOUND)
endif (USE_LIBPNG)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
  #GCC >= 7.2 needs -Wno-psabi
  add_compile_options(-Wno-psabi)
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
  add_compile_options(-Wno-inconsistent-missing-override)
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL AppleClang)
  add_compile_options(-Wno-inconsistent-missing-override)
endif()

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
  ${PLATFORM_SOURCES}
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
	infodlg.cpp
	main.cpp
	mapcanvas.cpp
	map.cpp
	messagelist.cpp
	regioninfo.cpp
	prefsdlg.cpp
	properties.cpp
	building_props.cpp
	ship_props.cpp
	regionpanel.cpp
	regionlist.cpp
	reportinfo.cpp
	searchdlg.cpp
	statistics.cpp
	statspanel.cpp
	symbols.cpp
	tradepanel.cpp
	unitlist.cpp
)

if (WIN32)
set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/version.rc")
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${APP_ICON_RESOURCE_WINDOWS})
else (WIN32)
add_executable(${PROJECT_NAME} ${SOURCES})
endif (WIN32)

if (CURL_FOUND)
add_compile_definitions(HAVE_CURL)
target_link_libraries(${PROJECT_NAME} PRIVATE
  ${CURL_LIBRARIES}
)
include_directories(
  ${CURL_INCLUDE_DIRS}
)
endif(CURL_FOUND)

if (PHYSFS_FOUND)
add_compile_definitions(HAVE_PHYSFS)
target_link_libraries(${PROJECT_NAME} PRIVATE
  ${PHYSFS_LIBRARY}
)
include_directories(
  ${PHYSFS_INCLUDE_DIRS}
)
endif(PHYSFS_FOUND)

if (PNG_FOUND)
add_compile_definitions(HAVE_PNG)
target_link_libraries(${PROJECT_NAME} PRIVATE
  PNG::PNG
)
include_directories(
  ${PNG_INCLUDE_DIRS}
)
endif(PNG_FOUND)

find_library(LEGACY_STDIO_LIBRARY NAMES legacy_stdio_definitions)
if (LEGACY_STDIO_LIBRARY)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LEGACY_STDIO_LIBRARY})
endif ()

find_package(FOX 1.6 QUIET CONFIG)
if (NOT FOX_FOUND)
include (FindPkgConfig)
if (PKG_CONFIG_FOUND)
pkg_search_module(FOX REQUIRED fox>=1.6)
else (PKG_CONFIG_FOUND)
find_path(FOX_INCLUDE_DIRS "fxkeys.h" REQUIRED)
find_library(FOX_LIBRARY NAMES FOX-1.6 REQUIRED)
endif (PKG_CONFIG_FOUND)
endif (NOT FOX_FOUND)
if (NOT FOX_LINK_LIBRARIES)
set (FOX_LINK_LIBRARIES ${FOX_LIBRARY})
endif (NOT FOX_LINK_LIBRARIES)

if (WIN32)
add_compile_definitions(FLOAT_MATH_FUNCTIONS)
endif (WIN32)

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 11)
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD_REQUIRED 11)
target_link_libraries(${PROJECT_NAME} PRIVATE
  ${FOX_LINK_LIBRARIES}
  ${CMAKE_DL_LIBS} 
)
include_directories(
	${FOX_INCLUDE_DIRS}
)

set(INSTALL_BIN_DIR "bin" CACHE STRING "Install executable in this directory")
set(INSTALL_DATA_DIR "share/csmapfx" CACHE STRING "Install data to this directory")
set(INSTALL_APPLICATIONS_DIR "share/applications" CACHE STRING "Install the .desktop file to this directory")


install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_BIN_DIR})
install(FILES csmapfx.xpm DESTINATION ${INSTALL_DATA_DIR})
install(FILES csmapfx.desktop DESTINATION ${INSTALL_APPLICATIONS_DIR})
