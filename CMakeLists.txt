#
# FAST Protect GmbH: SGI
#
# Project is hosted on:
# https://github.com/aroth-fastprotect/sgi
#
#
# History: 
# 2015-02-16  ARO  initial release

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0 FATAL_ERROR) 

# Include custom CMake modules or macros. Don't touch it.
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../buildtools/CMakeModules;${CMAKE_MODULE_PATH}")
INCLUDE(FastProtectMacros)

FAST_PROJECT(SGI)

FAST_IS_SYSTEM_LIBRARY(sgi SGI_IS_SYSTEM_LIB)
IF(NOT SGI_IS_SYSTEM_LIB)

	SET(SGI_MAJOR_VERSION 0)
	SET(SGI_MINOR_VERSION 3)
	SET(SGI_PATCH_VERSION 0)
	SET(SGI_SOVERSION     0)

	SET(SGI_PLUGIN_PREFIX "")

	IF (CYGWIN)
		SET(SGI_PLUGIN_PREFIX "cygwin_")
	ENDIF()

	IF(MINGW)
		SET(SGI_PLUGIN_PREFIX "mingw_")
	ENDIF()

	SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

	SET(SGI_VERSION ${SGI_MAJOR_VERSION}.${SGI_MINOR_VERSION}.${SGI_PATCH_VERSION})

	SET(SGI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/sgi)

	# Okay, here's the problem: On some platforms, linking against OpenThreads
	# is not enough and explicit linking to the underlying thread library
	# is also required (e.g. FreeBSD). But OpenThreads may be built with different
	# backends (Pthreads, Sproc, Windows) so we don't know what the underlying
	# thread library is because some platforms support multiple backends (e.g.
	# IRIX supports Sproc and Pthreads). Linking all libraries won't work
	# because the libraries may be incompatible.
	# So the current solution is to attempt best guess linking and exempt certain
	# cases. With IRIX, we're going to hope explicit linking to the underlying
	# library is not necessary. We currently don't case for pthreads on Windows
	# which might be an issue on things like Cygwin. This may need to be fixed.
	FIND_PACKAGE(Threads)
	IF(CMAKE_SYSTEM MATCHES IRIX)
		# Erase CMAKE_THREAD_LIBS_INIT and hope it works
		SET(CMAKE_THREAD_LIBS_INIT "" CACHE INTERNAL "")
	ENDIF(CMAKE_SYSTEM MATCHES IRIX)

	FIND_PACKAGE(OpenGL)
	FAST_PACKAGE_QT4()
	FAST_PACKAGE_LOG4CPLUS()

	IF(WIN32)
	  IF(MSVC)
			# This option is to enable the /MP switch for Visual Studio 2005 and above compilers
			OPTION(WIN32_USE_MP "Set to ON to build osgEarth with the /MP option (Visual Studio 2005 and above)." OFF)
			MARK_AS_ADVANCED(WIN32_USE_MP)
			IF(WIN32_USE_MP)
				SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
			ENDIF(WIN32_USE_MP)

			# turn off various warnings
			# foreach(warning 4244 4251 4267 4275 4290 4786 4305 4996)
			#     SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd${warning}")
			# endforeach(warning)

			# More MSVC specific compilation flags
			ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
			ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
			ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
		ENDIF(MSVC)
	ENDIF(WIN32)

	IF(CMAKE_COMPILER_IS_GNUCXX)
		ADD_DEFINITIONS(-std=c++11)
	ELSEIF(CLANG)
		ADD_DEFINITIONS(-std=c++11 -Wno-deprecated-register -Wno-switch)
	ENDIF()

	FAST_PACKAGE_OSG()
	#FAST_PACKAGE_OSGEARTH()

	# Make the headers visible to everything
	INCLUDE_DIRECTORIES(
		${OSG_INCLUDE_DIR}
		${QT_INCLUDES}
		${SGI_INCLUDE_DIR}
	)

	INCLUDE(SGIMacros)

	SET(OSG_PLUGINS osgPlugins-${OPENSCENEGRAPH_VERSION})

	OPTION(SGI_PLUGIN_OSG "Enable OSG plugin" ON)
	OPTION(SGI_PLUGIN_OSGEARTH "Enable osgEarth plugin" ON)
	OPTION(SGI_PLUGIN_QT "Enable Qt plugin" ON)
	OPTION(SGI_PLUGIN_LOG4CPLUS "Enable log4cplus plugin" OFF)

	# SGI Core
	ADD_SUBDIRECTORY(src)

	# SGI Plugins
	ADD_SUBDIRECTORY(plugins)

	add_subdirectory(applications)

ENDIF(NOT SGI_IS_SYSTEM_LIB)