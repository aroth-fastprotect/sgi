#
# FAST Protect GmbH: SGI
#
# Project is hosted on:
# https://github.com/aroth-fastprotect/sgi
#
#
# History: 
# 2015-02-16  ARO  initial release
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11 FATAL_ERROR)

if(COMMAND cmake_policy)
    # Works around warnings libraries linked against that don't
    # have absolute paths (e.g. -lpthreads)
    cmake_policy(SET CMP0003 NEW)

    # Works around warnings about escaped quotes in ADD_DEFINITIONS
    # statements.
    cmake_policy(SET CMP0005 OLD)

    # Qt5 qt5_use_modules usage was causing "Policy CMP0043 is not set: Ignore COMPILE_DEFINITIONS_<Config> properties." warnings
    cmake_policy(SET CMP0043 NEW)

endif(COMMAND cmake_policy)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#
# Set up CMake to use Solution Folders in VS.
#
SET_PROPERTY( GLOBAL PROPERTY USE_FOLDERS               ON )
SET_PROPERTY( GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets" )

PROJECT(SGI)

	SET(SGI_MAJOR_VERSION 0)
	SET(SGI_MINOR_VERSION 3)
	SET(SGI_PATCH_VERSION 0)
	SET(SGI_BUILD_VERSION 0)
	SET(SGI_SOVERSION     0)

	SET(SGI_PLUGIN_PREFIX "")

	IF (CYGWIN)
		SET(SGI_PLUGIN_PREFIX "cygwin_")
	ENDIF()

	IF(MINGW)
		SET(SGI_PLUGIN_PREFIX "mingw_")
	ENDIF()

	SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
	include(SGIFastMacros)

	SET(SGI_VERSION ${SGI_MAJOR_VERSION}.${SGI_MINOR_VERSION}.${SGI_PATCH_VERSION})

	SET(SGI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

	FIND_PACKAGE(OpenGL)

    # Find the QtWidgets library
    find_package(Qt5Widgets)
    find_package(Qt5OpenGL)

    set(QT_QTCORE_LIBRARY Qt5::Core )
    set(QT_QTGUI_LIBRARY Qt5::Gui)
    set(QT_QTWIDGETS_LIBRARY Qt5::Widgets )
    set(QT_QTOPENGL_LIBRARY Qt5::OpenGL )


    find_package(Log4cplus)

	find_package(OpenSceneGraph 3.2.0 REQUIRED osgDB osgGA osgUtil osgText osgFX osgManipulator osgViewer osgWidget osgTerrain osgAnimation)

	find_package(OsgEarth)
	find_package(OsgQt)

	IF(WIN32)
        # More MSVC specific compilation flags
        ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS)
	ENDIF(WIN32)

	IF(CMAKE_COMPILER_IS_GNUCXX)
		add_compile_options(-std=c++11)
	ELSEIF(CLANG)
		add_compile_options(-std=c++11 -Wno-deprecated-register -Wno-switch -Wno-inconsistent-missing-override)
	ENDIF()

	# Make the headers visible to everything
	INCLUDE_DIRECTORIES(
		${OSG_INCLUDE_DIR}
		${QT_INCLUDES}
	)
	INCLUDE_DIRECTORIES(BEFORE
		${SGI_INCLUDE_DIR}
		)
    INCLUDE(SGIFunctions)
	fast_configure_file(CMakeModules/SGIMacros.cmake.in ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/SGIMacros.cmake @ONLY)
	INCLUDE(SGIMacros)
	install(FILES CMakeModules/SGIMacros.cmake DESTINATION share/cmake/modules COMPONENT "Developer files")

	OPTION(SGI_PLUGIN_OSG "Enable OSG plugin" ON)
	OPTION(SGI_PLUGIN_OSGEARTH "Enable osgEarth plugin" ${OSGEARTH_FOUND})
	OPTION(SGI_PLUGIN_QT "Enable Qt plugin" ON)
	OPTION(SGI_PLUGIN_LOG4CPLUS "Enable log4cplus plugin" ${LOG4CPLUS_FOUND})
    OPTION(SGI_UNITTESTS "Enable SGI unitests" ON)
    OPTION(SGI_USE_FFMPEG "Use ffmpeg/swscale for image operations" ON)
	IF(WIN32)
		SET(SGI_SWSCALE_LIBRARYNAME "swscale-4" CACHE STRING "name of the swscale library")
	ELSE()
		SET(SGI_SWSCALE_LIBRARYNAME "swscale" CACHE STRING "name of the swscale library")
	ENDIF()
    OPTION(SGI_USE_GAMMARAY "Use gammary framework" OFF)

	SET(SGI_PLUGIN_IMPL_LIBRARY osgSGI-plugin-impl)

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
    fast_configure_file(${CMAKE_CURRENT_SOURCE_DIR}/SGIHostSupport.in ${SGI_INCLUDE_DIR}/sgi/plugins/SGIHostSupport)

    #FAST_INSTALL_EXTRA_HEADER_FILES(DIRECTORY ${SGI_INCLUDE_DIR}/sgi DESTINATION include/sgi)

    if(SGI_USE_GAMMARAY)
        add_definitions(-DSGI_USE_GAMMARAY=1)
        find_package(Gammaray REQUIRED)
    endif()

    if(OSGEARTH_FOUND)
        add_definitions(-DSGI_USE_OSGEARTH=1)
    endif()

    if(OSGQT_FOUND)
        add_definitions(-DSGI_USE_OSGQT=1)
    endif()

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
