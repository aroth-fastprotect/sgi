#
# FAST Protect GmbH: SGI
#
# Project is hosted on:
# https://github.com/aroth-fastprotect/sgi
#
#
# History: 
# 2015-02-16  ARO  initial release

CMAKE_MINIMUM_REQUIRED(VERSION 3.8.0 FATAL_ERROR)

# Include custom CMake modules or macros. Don't touch it.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../buildtools/CMakeModules")
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../buildtools/CMakeModules;${CMAKE_MODULE_PATH}")
    include(FastProtectMacros)
else()
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
    include(SGIFastMacros)
endif()

FAST_PROJECT(SGI)

FAST_IS_SYSTEM_LIBRARY(sgi SGI_IS_SYSTEM_LIB)
IF(NOT SGI_IS_SYSTEM_LIB)

    set(CMAKE_INCLUDE_CURRENT_DIR ON)

	SET(SGI_VERSION_MAJOR 0)
    SET(SGI_VERSION_MINOR 4)
	SET(SGI_VERSION_PATCH 0)
	SET(SGI_VERSION_BUILD 0)
	SET(SGI_SOVERSION     0)
	SET(SGI_COPYRIGHT_TEXT "Copyright (C) 2012-2019")

    SET(SGI_PLUGIN_PREFIX "")

	IF (CYGWIN)
		SET(SGI_PLUGIN_PREFIX "cygwin_")
	ENDIF()

	IF(MINGW)
		SET(SGI_PLUGIN_PREFIX "mingw_")
	ENDIF()

	set(SGI_CMAKEMODULES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

	SET(SGI_VERSION ${SGI_MAJOR_VERSION}.${SGI_MINOR_VERSION}.${SGI_PATCH_VERSION})

	SET(SGI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

    if(OSG_DIR)
        set(OSG_DIR "${OSG_DIR}" CACHE STRING "Directory where to find OSG")
    endif()
    if(OSGEARTH_DIR)
        set(OSGEARTH_DIR "${OSGEARTH_DIR}" CACHE STRING "Directory where to find osgEarth")
    endif()
    if(OSGQT_DIR)
        set(OSGQT_DIR "${OSGQT_DIR}" CACHE STRING "Directory where to find osgQt")
    endif()

	fast_package_opengl()
	fast_package_qt5()
	fast_package_osg()
    fast_package_log4cplus()

    if(OSGEARTH_FOUND AND EXISTS ${OSGEARTH_INCLUDE_DIR}/osgEarth/Version)
        file (READ ${OSGEARTH_INCLUDE_DIR}/osgEarth/Version _osgearth_version_header_contents)
        IF(_osgearth_version_header_contents)
            string (REGEX MATCH "OSGEARTH_MAJOR_VERSION[ \t]+([0-9]+)" OSGEARTH_MAJOR_VERSION ${_osgearth_version_header_contents})
            string (REGEX MATCH "([0-9]+)" OSGEARTH_MAJOR_VERSION ${OSGEARTH_MAJOR_VERSION})

            string (REGEX MATCH "OSGEARTH_MINOR_VERSION[ \t]+([0-9]+)" OSGEARTH_MINOR_VERSION ${_osgearth_version_header_contents})
            string (REGEX MATCH "([0-9]+)" OSGEARTH_MINOR_VERSION ${OSGEARTH_MINOR_VERSION})

            string (REGEX MATCH "OSGEARTH_PATCH_VERSION[ \t]+([0-9]+)" OSGEARTH_PATCH_VERSION ${_osgearth_version_header_contents})
            string (REGEX MATCH "([0-9]+)" OSGEARTH_PATCH_VERSION ${OSGEARTH_PATCH_VERSION})

            string (REGEX MATCH "OSGEARTH_SOVERSION[ \t]+([0-9]+)" OSGEARTH_SOVERSION ${_osgearth_version_header_contents})
            string (REGEX MATCH "([0-9]+)" OSGEARTH_SOVERSION ${OSGEARTH_SOVERSION})
            SET(OSGEARTH_VERSION ${OSGEARTH_MAJOR_VERSION}.${OSGEARTH_MINOR_VERSION}.${OSGEARTH_PATCH_VERSION})
        endif()
    endif()

	IF(WIN32)
        # More MSVC specific compilation flags
        ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DNOMINMAX)
	ENDIF(WIN32)

    # Use C++14 as default
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    # ... and keep C++ extensions on (which is the CMake default, but to be safe)
    set(CMAKE_CXX_EXTENSIONS ON)
	IF(CLANG)
		add_compile_options(-Wno-deprecated-register -Wno-switch -Wno-inconsistent-missing-override -Wno-enum-compare-switch)
	ENDIF()

	# Make the headers visible to everything
	INCLUDE_DIRECTORIES(
		${OSG_INCLUDE_DIR}
        ${OSG_GLCORE_INCLUDE_DIR}
		${QT_INCLUDES}
	)
	INCLUDE_DIRECTORIES(BEFORE
		${SGI_INCLUDE_DIR}
		)
    INCLUDE(${SGI_CMAKEMODULES_DIR}/SGIFunctions.cmake)
	fast_configure_file(CMakeModules/SGIMacros.cmake.in ${SGI_CMAKEMODULES_DIR}/SGIMacros.cmake @ONLY)
	INCLUDE(${SGI_CMAKEMODULES_DIR}/SGIMacros.cmake)
    FAST_INSTALL_DATA_FILE(${SGI_CMAKEMODULES_DIR}/SGIMacros.cmake DESTINATION share/cmake/modules COMPONENT "Developer files")

	OPTION(SGI_PLUGIN_OSG "Enable OSG plugin" ON)
	OPTION(SGI_PLUGIN_OSGEARTH "Enable osgEarth plugin" ${OSGEARTH_FOUND})
	OPTION(SGI_PLUGIN_QT "Enable Qt plugin" ON)
    OPTION(SGI_PLUGIN_QTQUICK "Enable QtQuick plugin" ON)
	OPTION(SGI_PLUGIN_LOG4CPLUS "Enable log4cplus plugin" ${LOG4CPLUS_FOUND})
    OPTION(SGI_UNITTESTS "Enable SGI unitests" OFF)
    OPTION(SGI_BUILD_GLIMAGEVIEW "Build the GLImageView application" OFF)
    OPTION(SGI_USE_OSGQT "Use osgQt module (if available)" ${OSGQT_FOUND})
    OPTION(SGI_USE_FFMPEG "Use ffmpeg/swscale for image operations" ON)
    if(WIN32 AND FAST_SGI_CMAKE_SYSTEM)
        set(_sgi_generate_qt_conf_default ON)
    else()
        set(_sgi_generate_qt_conf_default OFF)
    endif()
    OPTION(SGI_GENERATE_QT_CONF "Generate qt.conf in output binary directory to find the Qt framework" ${_sgi_generate_qt_conf_default})
    if(WIN32)
        set(_sgi_swscale_use_library_postfix_default ON)
    else()
        set(_sgi_swscale_use_library_postfix_default OFF)
    endif()
    OPTION(SGI_SWSCALE_USE_LIBRARY_POSTFIX "Use cmake library postfix for swscale" ${_sgi_swscale_use_library_postfix_default})
	IF(WIN32)
		SET(SGI_SWSCALE_LIBRARYNAME "swscale-4" CACHE STRING "name of the swscale library")
	ELSE()
		SET(SGI_SWSCALE_LIBRARYNAME "swscale" CACHE STRING "name of the swscale library")
	ENDIF()
    OPTION(SGI_USE_GAMMARAY "Use gammary framework" OFF)
    OPTION(SGI_USE_FLIGHTGEAR_GW "Use flightgear graphicswindow" OFF)

	SET(SGI_PLUGIN_IMPL_LIBRARY sgi-plugin-impl)
    set(SGI_OSG_HELPER_LIBRARY sgi-osg-helper)
    set(SGI_QT_HELPER_LIBRARY sgi-qt-helper)
    set(SGI_REF_LIBRARY sgi-ref)

    set(SGI_PLUGIN_HOST_SUPPORT_INCLUDES)
    set(SGI_PLUGIN_HOST_SUPPORT_ITEM_CLASSES)
    set(SGI_PLUGIN_FALLBACK ON)
    SGI_PLUGIN_HOST_SUPPORT(osg Osg)
    SGI_PLUGIN_HOST_SUPPORT(osgearth OsgEarthConfig OsgEarthConfigOptions)
    SGI_PLUGIN_HOST_SUPPORT(log4cplus Log4cplus)
    SGI_PLUGIN_HOST_SUPPORT(qt Qt QtMeta)
    SGI_PLUGIN_HOST_SUPPORT(fallback Fallback)

    TO_LIST_NEWLINES(SGI_PLUGIN_HOST_SUPPORT_INCLUDES SGI_PLUGIN_HOST_SUPPORT_INCLUDES)
    TO_LIST_COMMA(SGI_PLUGIN_HOST_SUPPORT_ITEM_CLASSES SGI_PLUGIN_HOST_SUPPORT_ITEM_CLASSES)

    FAST_INSTALL_EXTRA_HEADER_FILES(DIRECTORY ${SGI_INCLUDE_DIR}/sgi DESTINATION include/sgi)

    if(LOG4CPLUS_FOUND)
        set(SGI_HAS_LOG4CPLUS_SUPPORT 1)
    endif()

    if(SGI_USE_GAMMARAY)
        find_package(Gammaray)
        if(Gammaray_FOUND)
            add_definitions(-DSGI_USE_GAMMARAY=1)
            set(SGI_HAS_GAMMARAY_SUPPORT 1)
        endif()
    endif()

    if(SGI_USE_FFMPEG)
        find_package(FFmpeg)
        if(FFMPEG_FOUND)
            add_definitions(-DSGI_USE_FFMPEG=1)
            set(SGI_HAS_FFMPEG_SUPPORT 1)
        endif()
    endif()

    if(OSGEARTH_FOUND)
        set(SGI_HAS_OSGEARTH_SUPPORT 1)
        add_definitions(-DSGI_USE_OSGEARTH=1)
    endif()

    if(OSGQT_FOUND AND SGI_USE_OSGQT)
        set(SGI_HAS_OSGQT_SUPPORT 1)
        add_definitions(-DSGI_USE_OSGQT=1)
    endif()

    fast_configure_file(${CMAKE_CURRENT_SOURCE_DIR}/SGIHostSupport.in ${SGI_INCLUDE_DIR}/sgi/plugins/SGIHostSupport)

	# SGI Core
	ADD_SUBDIRECTORY(src)

	# SGI Plugins
	ADD_SUBDIRECTORY(plugins)

	# Use gammaray framework
	if(SGI_USE_GAMMARAY)
        add_subdirectory(gammaray)
    endif()

	add_subdirectory(applications)
    if(SGI_UNITTESTS)
        add_subdirectory(tests)
    endif()

	FAST_PROJECT_PACKAGE()

ENDIF(NOT SGI_IS_SYSTEM_LIB)
