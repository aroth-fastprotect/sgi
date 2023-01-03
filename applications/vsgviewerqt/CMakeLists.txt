include_directories(../viewer_base_vsg)

SET( TARGET_SRC
    vsgviewerQt.cpp
)

SET(TARGET_MOC_H
    vsgviewerQt.h
)
SET(TARGET_H
)

SET(TARGET_LIBRARIES_VARS
    sgi_viewer_base_vsg
)
LIST(APPEND TARGET_LIBRARIES_VARS
    vsgQt::vsgQt
)

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

if(FAST_SGI_CMAKE_SYSTEM)
    file(RELATIVE_PATH _qt_plugin_dir ${FAST_WORK_BIN_DIR} ${FAST_WORK_PLUGINS_DIR})
    add_definitions(-DSGI_QT_PLUGIN_DIR="/${_qt_plugin_dir}")
else()
    VAR_CONFIG(_postfix FAST_POSTFIX)
    add_definitions(-DSGI_QT_PLUGIN_DIR="/../plugins${_postfix}")
endif()

FAST_SET_TARGET_AND_PRODUCT_VERSION(SGI)
SET(TARGET_NO_TRANSLATION 1)

FAST_COMMANDLINE_APPLICATION( sgi_vsgviewerQt NO_MOC_INCLUDE )

