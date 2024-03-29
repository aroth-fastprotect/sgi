include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Widgets_PRIVATE_INCLUDE_DIRS})

SET(TARGET_SRC
    stdafx.cpp
    sgi_qt_plugin.cpp
    writeHTMLQt.cpp
    writeHTMLQtMeta.cpp
    writeHTMLQtPaintDevice.cpp
    writeHTMLQtQPA.cpp
    getObjectInfoQt.cpp
    ObjectTreeQt.cpp
    ContextMenuQt.cpp
    MenuActionQt.cpp
    SettingsDialogQt.cpp
    )

SET(TARGET_H
    SGIItemQt
    stdafx.h
    writeHTMLQt.h
    getObjectInfoQt.h
    ObjectTreeQt.h
    ContextMenuQt.h
    MenuActionQt.h
    SettingsDialogQt.h
    )

OPTION(SGI_PLUGIN_QT_WITH_QTOPENGL "Enable support for QtOpenGL (deprecated)" ON)
IF(SGI_PLUGIN_QT_WITH_QTOPENGL)
    LIST(APPEND TARGET_DEFINITIONS WITH_QTOPENGL)
ENDIF()

SGI_CREATE_SYMLINK(SGIItemQt ${SGI_INCLUDE_DIR}/sgi/plugins/SGIItemQt)

LIST(APPEND TARGET_LIBRARIES_VARS
    SGI_QT_HELPER_LIBRARY
    QT_QTCORE_LIBRARY
    QT_QTGUI_LIBRARY
    QT_QTWIDGETS_LIBRARY
    )
IF ( SGI_PLUGIN_QT_WITH_QTOPENGL )
    LIST(APPEND TARGET_LIBRARIES_VARS QT_QTOPENGL_LIBRARY )
ENDIF()

SGI_PLUGIN(qt)
