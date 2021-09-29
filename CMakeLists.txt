cmake_minimum_required(VERSION 2.9)
project (csmapfx CXX C)

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

set (SOURCES
	calc.cpp
	commands.cpp
	csmap.cpp
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
	regioninfos.cpp
	regionlist.cpp
	scout.cpp
	searchdlg.cpp
	statistics.cpp
	statsinfos.cpp
	symbols.cpp
	tradeinfos.cpp
  translate.cpp
  unitlist.cpp
	tinyjs/TinyJS.cpp
	tinyjs/TinyJS_Functions.cpp
	tinyjs/TinyJS_MathFunctions.cpp
)

if (WIN32)
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
else (WIN32)
add_executable(${PROJECT_NAME} ${SOURCES})
endif (WIN32)

find_package(PhysFS 2.1 REQUIRED)
find_package(PNG REQUIRED)
find_package(CURL REQUIRED)
find_package(Intl REQUIRED)

find_library(LEGACY_STDIO_LIBRARY NAMES legacy_stdio_definitions)
if (LEGACY_STDIO_LIBRARY)
target_link_libraries(${PROJECT_NAME} ${LEGACY_STDIO_LIBRARY})
endif ()

find_package(Gettext)
include_directories(${Intl_INCLUDE_DIR})
if (Intl_FOUND)
  target_compile_definitions (${PROJECT_NAME} PRIVATE HAVE_INTL=1)
  target_link_libraries(${PROJECT_NAME} ${Intl_LIBRARIES})
else (Intl_FOUND)
  message(STATUS "libintl not found")
endif (Intl_FOUND)

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
)
if (FOX_LIBRARY_DIRS)
  target_link_libraries(${PROJECT_NAME} -L${FOX_LIBRARY_DIRS})
endif(FOX_LIBRARY_DIRS)
include_directories(
  ${PHYSFS_INCLUDE_DIR}
	${PNG_INCLUDE_DIRS}
  ${CURL_INCLUDE_DIRS}
	${FOX_INCLUDE_DIRS}
)

include(GNUInstallDirs)
set(PREFIX_BIN ${CMAKE_INSTALL_BINDIR} CACHE STRING "Install executable in this directory")
set(PREFIX_DATA ${CMAKE_INSTALL_DATADIR} CACHE STRING "Install data to this directory")
set(PREFIX_PKGDATA ${PREFIX_DATA}/${PACKAGE_NAME})
set(PREFIX_LOCALE ${CMAKE_INSTALL_DATADIR}/locale)
set(PREFIX_APPDATA ${CMAKE_INSTALL_DATADIR}/applications)

include(Localization.cmake)
SET (PACKAGE_NAME ${PROJECT_NAME})
PROGRAM_LOCALIZATION()

install(TARGETS ${PROJECT_NAME} DESTINATION ${PREFIX_BIN})
install(FILES csmapfx.xpm DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
install(FILES csmapfx.desktop DESTINATION ${PREFIX_APPDATA})
