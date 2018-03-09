SET( TARGET_SRC
    MainWindow.cpp
)

SET(TARGET_MOC_H
    MainWindow.h
)
SET( TARGET_UI
    MainWindow.ui
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


set(_imagewidget_dir ../../src/sgi)

include_directories(${_imagewidget_dir})

set(SHADERS_CPP "${CMAKE_CURRENT_BINARY_DIR}/${_imagewidget_dir}/AutoGenShaders.cpp")

list(APPEND TARGET_SRC
    ${_imagewidget_dir}/ImageGLWidget.cpp
    ${SHADERS_CPP}
)

list(APPEND TARGET_MOC_H
    ${_imagewidget_dir}/ImageGLWidget.h
)


LIST(APPEND TARGET_LIBRARIES_VARS
    OSG_LIBRARY
    SGI_PLUGIN_IMPL_LIBRARY
    SGI_QT_HELPER_LIBRARY
    )

VAR_CONFIG(_postfix FAST_POSTFIX)
add_definitions(-DSGI_QT_PLUGIN_DIR="/../plugins${_postfix}")

FAST_SET_TARGET_AND_PRODUCT_VERSION(SGI)
SET(TARGET_NO_TRANSLATION 1)

FAST_COMMANDLINE_APPLICATION( glimageview NO_MOC_INCLUDE )

