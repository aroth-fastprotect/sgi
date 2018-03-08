# This module defines

# OSGQT_LIBRARY
# OSGQT_FOUND, if false, do not try to link to osg
# OSGQT_INCLUDE_DIRS, where to find the headers
# OSGQT_INCLUDE_DIR, where to find the source headers

# to use this module, set variables to point to the osg build
# directory, and source directory, respectively
# OSGQTDIR or OSGQT_SOURCE_DIR: osg source directory, typically OpenSceneGraph
# OSGQT_DIR or OSGQT_BUILD_DIR: osg build directory, place in which you've
#    built osg via cmake

# Header files are presumed to be included like
# #include <osg/PositionAttitudeTransform>
# #include <osgUtil/SceneView>

###### headers ######

MACRO( FIND_OSGQT_INCLUDE THIS_OSGQT_INCLUDE_DIR THIS_OSGQT_INCLUDE_FILE )

FIND_PATH( ${THIS_OSGQT_INCLUDE_DIR} ${THIS_OSGQT_INCLUDE_FILE}
    PATHS
        ${OSGQT_DIR}
        $ENV{OSGQT_SOURCE_DIR}
        $ENV{OSGQTDIR}
        $ENV{OSGQT_DIR}
        /usr/local/
        /usr/
        /sw/ # Fink
        /opt/local/ # DarwinPorts
        /opt/csw/ # Blastwave
        /opt/
        [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSGQT_ROOT]/
        ~/Library/Frameworks
        /Library/Frameworks
    PATH_SUFFIXES
        /include/
)

ENDMACRO( FIND_OSGQT_INCLUDE THIS_OSGQT_INCLUDE_DIR THIS_OSGQT_INCLUDE_FILE )

FIND_OSGQT_INCLUDE( OSGQT_INCLUDE_DIR       osgQt/GraphicsWindowQt )

###### libraries ######

MACRO( FIND_OSGQT_LIBRARY MYLIBRARY MYLIBRARYNAME )

FIND_LIBRARY(${MYLIBRARY}
    NAMES
        ${MYLIBRARYNAME}
    PATHS
        ${OSGQT_DIR}
        $ENV{OSGQT_BUILD_DIR}
        $ENV{OSGQT_DIR}
        $ENV{OSGQTDIR}
        $ENV{OSG_ROOT}
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local
        /usr
        /sw
        /opt/local
        /opt/csw
        /opt
        [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSGQT_ROOT]/lib
        /usr/freeware
    PATH_SUFFIXES
        /lib/
        /lib64/
        /build/lib/
        /build/lib64/
        /Build/lib/
        /Build/lib64/
     )

ENDMACRO(FIND_OSGQT_LIBRARY LIBRARY LIBRARYNAME)

FIND_OSGQT_LIBRARY( OSGQT_LIBRARY osgqt)

SET( OSGQT_FOUND "NO" )
IF( OSGQT_LIBRARY AND OSGQT_INCLUDE_DIR )
    SET( OSGQT_FOUND "YES" )
    SET( OSGQT_INCLUDE_DIRS ${OSGQT_INCLUDE_DIR})
    GET_FILENAME_COMPONENT( OSGQT_LIBRARIES_DIR ${OSGQT_LIBRARY} PATH )
ENDIF( OSGQT_LIBRARY AND OSGQT_INCLUDE_DIR )
