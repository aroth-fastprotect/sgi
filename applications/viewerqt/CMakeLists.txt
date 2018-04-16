include_directories(../viewer_base)

SET( TARGET_SRC
    osgviewerQt.cpp
)

SET(TARGET_MOC_H
    osgviewerQt.h
)
SET(TARGET_H
)

SET(TARGET_LIBRARIES_VARS
    OPENTHREADS_LIBRARY
    OSG_LIBRARY
    OSGDB_LIBRARY
    OSGUTIL_LIBRARY
    OSGGA_LIBRARY
    OSGVIEWER_LIBRARY
    sgi_viewer_base
)
if(OSGQT_FOUND)
    include_directories(${OSGQT_INCLUDE_DIR})
    LIST(APPEND TARGET_LIBRARIES_VARS
        OSGQT_LIBRARY
    )
endif()

LIST(APPEND TARGET_LIBRARIES_VARS
    QT_QTCORE_LIBRARY
    QT_QTGUI_LIBRARY
    )
IF ( Qt5Widgets_FOUND )
    # Add the include directories for the Qt 5 Widgets module to
    # the compile lines.
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
	LIST(APPEND TARGET_LIBRARIES_VARS QT_QTWIDGETS_LIBRARY )
ENDIF()
IF ( Qt5OpenGL_FOUND )
    # Add the include directories for the Qt 5 OpenGL module to
    # the compile lines.
    include_directories(${Qt5OpenGL_INCLUDE_DIRS})
	LIST(APPEND TARGET_LIBRARIES_VARS QT_QTOPENGL_LIBRARY )
ENDIF()

IF(OSGEARTH_FOUND)
    include_directories(${OSGEARTH_INCLUDE_DIR})
    LIST(APPEND TARGET_LIBRARIES_VARS
        OSGEARTH_LIBRARY
        OSGEARTHUTIL_LIBRARY
        OSGEARTHSYMBOLOGY_LIBRARY
    )
ENDIF()

if(FAST_SGI_CMAKE_SYSTEM)
    file(RELATIVE_PATH _qt_plugin_dir ${FAST_WORK_BIN_DIR} ${FAST_WORK_PLUGINS_DIR})
    add_definitions(-DSGI_QT_PLUGIN_DIR="/${_qt_plugin_dir}")
else()
    VAR_CONFIG(_postfix FAST_POSTFIX)
    add_definitions(-DSGI_QT_PLUGIN_DIR="/../plugins${_postfix}")
endif()

FAST_SET_TARGET_AND_PRODUCT_VERSION(SGI)
SET(TARGET_NO_TRANSLATION 1)

FAST_COMMANDLINE_APPLICATION( sgi_osgviewerQt NO_MOC_INCLUDE )

