cmake_minimum_required(VERSION 2.8)
project (csmapfx CXX)

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
	translator.cpp
	unitlist.cpp
	tinyjs/TinyJS.cpp
	tinyjs/TinyJS_Functions.cpp
	tinyjs/TinyJS_MathFunctions.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

find_path(BOOST_CORE_INCLUDE_DIRS "boost/checked_delete.hpp")
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_CORE_INCLUDE_DIRS})
find_path(BOOST_FUNCTION_INCLUDE_DIRS "boost/function.hpp")
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_FUNCTION_INCLUDE_DIRS})
find_path(BOOST_TUPLE_INCLUDE_DIRS "boost/tuple/detail/tuple_basic.hpp")
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_TUPLE_INCLUDE_DIRS})

find_package(PhysFS REQUIRED)
find_package(PNG REQUIRED)

find_library(LEGACY_STDIO_LIBRARY NAMES legacy_stdio_definitions)
if (LEGACY_STDIO_LIBRARY)
target_link_libraries(${PROJECT_NAME} ${LEGACY_STDIO_LIBRARY})
endif ()

if (WIN32)
set(FOX_ROOT $ENV{FOX_ROOT})
if (NOT FOX_ROOT)
set (FOX_ROOT "c:/homebrew/fox-1.6.57")
endif (NOT FOX_ROOT)
find_path(FOX_INCLUDE_DIRS "fxkeys.h" PATHS "${FOX_ROOT}/include" REQUIRED)
find_library(FOX_LIBRARY NAMES FOX-1.6 PATHS "${FOX_ROOT}/lib" REQUIRED)
set (FOX_LIBRARIES ${FOX_LIBRARY})
add_compile_definitions(
BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
BOOST_SIGNALS_NO_DEPRECATION_WARNING
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
	${FOX_LIBRARIES}
)
include_directories(
  ${PHYSFS_INCLUDE_DIR}
	${PNG_INCLUDE_DIRS}
	${FOX_INCLUDE_DIRS}
)
