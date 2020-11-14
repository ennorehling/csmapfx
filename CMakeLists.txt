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

#find_package(Boost REQUIRED)
find_path(BOOST_CORE_INCLUDE_DIRS "boost/checked_delete.hpp")
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_CORE_INCLUDE_DIRS})
find_path(BOOST_FUNCTION_INCLUDE_DIRS "boost/function.hpp")
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_FUNCTION_INCLUDE_DIRS})
find_package(PNG REQUIRED)

include (FindPkgConfig)
if (PKG_CONFIG_EXECUTABLE)
pkg_search_module(FOX REQUIRED fox>=1.6)
else (PKG_CONFIG_EXECUTABLE)
find_path(FOX_INCLUDE_DIRS "fxkeys.h" PATHS c:/usrlocal/fox-1.6.57/include)
find_library(FOX_LIBRARIES NAMES FOX-1.6 FOXD-1.6 PATHS c:/usrlocal/fox-1.6.57/lib)
endif (PKG_CONFIG_EXECUTABLE)

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 11)
target_link_libraries(${PROJECT_NAME}
	${PNG_LIBRARIES}
	${FOX_LIBRARIES}
)
include_directories(
	${PNG_INCLUDE_DIRS}
	${FOX_INCLUDE_DIRS}
)
