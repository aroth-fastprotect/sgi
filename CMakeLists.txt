CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4 FATAL_ERROR)

if(COMMAND cmake_policy)
    # Works around warnings libraries linked against that don't
    # have absolute paths (e.g. -lpthreads)
    cmake_policy(SET CMP0003 NEW)

    # Works around warnings about escaped quotes in ADD_DEFINITIONS
    # statements.
    cmake_policy(SET CMP0005 OLD)

    # disable autolinking to qtmain as we have our own main() functions (new in Qt 5.1)
    if(NOT "${CMAKE_VERSION}" VERSION_LESS 2.8.11)
        cmake_policy(SET CMP0020 OLD)
    endif(NOT "${CMAKE_VERSION}" VERSION_LESS 2.8.11)
endif(COMMAND cmake_policy)

#
# Set up CMake to use Solution Folders in VS.
#
SET_PROPERTY( GLOBAL PROPERTY USE_FOLDERS               ON )
SET_PROPERTY( GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets" )

SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

PROJECT(SGI)

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


SET(SGI_VERSION ${SGI_MAJOR_VERSION}.${SGI_MINOR_VERSION}.${SGI_PATCH_VERSION})

SET(SGI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/sgi)

# We want to build SONAMES shared librariess
SET(SGI_SONAMES TRUE)
SET(OPENTHREADS_SONAMES TRUE)
SET(DYNAMIC_OSGEARTH TRUE)

SET(OpenThreads_SOURCE_DIR ${OSGEARTH_SOURCE_DIR})

# Good place to look for the OSG 3rd party libs
SET(THIRD_PARTY_DIR "" CACHE PATH "OSG 3rd-party dependency folder")

# We have some custom .cmake scripts not in the official distribution.
# Maybe this can be used override existing behavior if needed?
SET(CMAKE_MODULE_PATH "${SGI_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
    SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
ENDIF()

# Attempt to detect the GIT revision number.
OPTION(SGI_EMBED_GIT_SHA "Embeds the GIT SHA in the version code" OFF)
IF (SGI_EMBED_GIT_SHA)
  include(GetGitRevisionDescription)
  get_git_head_revision(GIT_REFSPEC SGI_GIT_SHA1)
ENDIF (SGI_EMBED_GIT_SHA)

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

OPTION(USE_QT4 "Set to ON to build SGI with Qt4." ON)

IF(NOT USE_QT4)
    FIND_PACKAGE(Qt5Core QUIET)
    FIND_PACKAGE(Qt5Widgets QUIET)
    FIND_PACKAGE(Qt5Gui QUIET)
    FIND_PACKAGE(Qt5OpenGL QUIET)
ENDIF()
IF ( Qt5Core_FOUND AND Qt5Widgets_FOUND AND Qt5Gui_FOUND AND Qt5OpenGL_FOUND )
    SET(QT_INCLUDES ${Qt5Widgets_INCLUDE_DIRS} ${Qt5OpenGL_INCLUDE_DIRS})
ELSE()
    FIND_PACKAGE(Qt4)
    IF (QT4_FOUND)
        INCLUDE(${QT_USE_FILE})
		SET(QT_INCLUDES ${QT_INCLUDES} ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}${QT_QTOPENGL_INCLUDE_DIR} )
        SET(QT_ALL_LIBRARIES ${QT_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTWEBKIT_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTOPENGL_LIBRARY})
    ENDIF (QT4_FOUND)
ENDIF ()

IF(UNIX)
    # Not sure what this will do on Cygwin and Msys
    # Also, remember OS X X11 is a user installed option so it may not exist.
    FIND_PACKAGE(X11)
    # Some Unicies need explicit linkage to the Math library or the build fails.
    FIND_LIBRARY(MATH_LIBRARY m)
ENDIF(UNIX)

# Common global definitions
#ADD_DEFINITIONS(-D)
# Platform specific definitions


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


########################################################################################################
##### these were settings located in SetupCommon.cmake used in Luigi builds.... find out what are useful
########################################################################################################
#luigi#SET(CMAKE_VERBOSE_MAKEFILE TRUE)
#luigi#SET(CMAKE_SKIP_RPATH TRUE)
#luigi#SET(CMAKE_SKIP_RULE_DEPENDENCY TRUE)
#luigi#IF(UNIX)
#luigi#    LIST_CONTAINS(contains "g++" ${CMAKE_CXX_COMPILER_LIST})
#luigi#    IF (contains)
#luigi#        MESSAGE(${MY_MESSAGE_DEFAULT} "${CMAKE_CURRENT_LIST_FILE}:${CMAKE_CURRENT_LIST_LINE} setting  CMAKE_CXX_COMPILER to g++")
#luigi#        SET(CMAKE_CXX_COMPILER "g++")
#luigi#        SET(CMAKE_CXX_COMPILER_LOADED 2)
#luigi#        SET(CMAKE_CXX_COMPILER_WORKS 2)
#luigi#    ENDIF (contains)
#luigi#    SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
#luigi#    SET(CMAKE_CXX_FLAGS_DEBUG "-ggdb -gstabs")
#luigi#ENDIF(UNIX)
########################################################################################################

# Common to all platforms:

SET(CMAKE_DEBUG_POSTFIX  "d" CACHE STRING "add a postfix, usually d on windows")
SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
SET(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "add a postfix, usually empty on windows")
SET(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "add a postfix, usually empty on windows")

SET(CMAKE_AUTOMOC OFF CACHE BOOL "disable cmake automoc")

FIND_PACKAGE(OSG)
FIND_PACKAGE(OsgEarth)

# Make the headers visible to everything
INCLUDE_DIRECTORIES(
    ${OSG_INCLUDE_DIR}
    ${QT_INCLUDES}
    ${SGI_INCLUDE_DIR}
)

################################################################################
# Create bin and lib directories if required

IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin ${CMAKE_BINARY_DIR}/lib)
ENDIF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")


################################################################################
# Installation stuff


IF(UNIX AND NOT WIN32 AND NOT APPLE)
  IF(CMAKE_SIZEOF_VOID_P MATCHES "8")
      SET(LIB_POSTFIX "64" CACHE STRING "suffix for 32/64 dir placement")
      MARK_AS_ADVANCED(LIB_POSTFIX)
  ENDIF(CMAKE_SIZEOF_VOID_P MATCHES "8")
ENDIF(UNIX AND NOT WIN32 AND NOT APPLE)
IF(NOT DEFINED LIB_POSTFIX)
    SET(LIB_POSTFIX "")
ENDIF(NOT DEFINED LIB_POSTFIX)

#SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME})
SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})
SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_BINDIR})

#SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib/${CMAKE_SYSTEM_NAME})
SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
MAKE_DIRECTORY(${OUTPUT_LIBDIR})
SET(LIBRARY_OUTPUT_PATH ${OUTPUT_LIBDIR})

LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${OUTPUT_LIBDIR} )


#Whether or not to copy osgEarth to the OSG_DIR during installation
# Dynamic vs Static Linking
OPTION(INSTALL_TO_OSG_DIR "Set to ON to install osgearth to the path defined in the OSG_DIR" OFF)
IF(INSTALL_TO_OSG_DIR)
    SET(OSGEARTH_INSTALL_TO_OSG_DIR TRUE)
ELSE (INSTALL_TO_OSG_DIR)
    SET(OSGEARTH_INSTALL_TO_OSG_DIR FALSE)
ENDIF(INSTALL_TO_OSG_DIR)


INCLUDE(OsgEarthMacroUtils)
INCLUDE(SGIMacros)

DETECT_OSG_VERSION()

SET(OPENSCENEGRAPH_VERSION ${OPENSCENEGRAPH_MAJOR_VERSION}.${OPENSCENEGRAPH_MINOR_VERSION}.${OPENSCENEGRAPH_PATCH_VERSION})
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

#-----------------------------------------------------------------------------
### uninstall target
#-----------------------------------------------------------------------------
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")



#------------------------------------------------------------------------------
# Docs.
#add_subdirectory(docs)



#------------------------------------------------------------------------------
# Packaging setup.

SET(CPACK_PACKAGE_VERSION       ${SGI_VERSION}       )
SET(CPACK_PACKAGE_VERSION_MAJOR ${SGI_MAJOR_VERSION} )
SET(CPACK_PACKAGE_VERSION_MINOR ${SGI_MINOR_VERSION} )
SET(CPACK_PACKAGE_VERSION_PATCH ${SGI_PATCH_VERSION} )
SET(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE.txt )
SET(CPACK_MONOLITHIC_INSTALL    1 )                                        # create a single, monolithic package.
    
INCLUDE(CPack)
