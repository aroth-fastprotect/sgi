IF ( Qt5Widgets_FOUND )
    # Add the include directories for the Qt 5 Widgets module to
    # the compile lines.
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
ENDIF()

IF(OSGEARTH_FOUND)
    include_directories(${OSGEARTH_INCLUDE_DIR})
ENDIF()
if(OSGQT_FOUND)
    include_directories(${OSGQT_INCLUDE_DIR})
endif()

set(TARGET_SRC 
    sgi_viewer_base.cpp
    ../../src/plugin_impl/osg_viewer.cpp
    )
list(APPEND TARGET_DEFINITIONS BUILD_SGI_VIEWER_BASE)
set(TARGET_H sgi_viewer_base.h)

FAST_SET_TARGET_AND_PRODUCT_VERSION(SGI)
SET(TARGET_NO_TRANSLATION 1)
SET(TARGET_NO_AUTO_VERSION_RESOURCE 1)
SET(TARGET_NO_AUTO_VERSION_HEADER 1)

if(UNIX)
    list(APPEND TARGET_ADDED_LIBRARIES dl)
endif()

FAST_STATIC_LIBRARY(sgi_viewer_base)
