SET(TARGET_SRC
    stdafx.cpp
    sgi_qt_plugin.cpp
    writeHTMLQt.cpp
    writeHTMLQtMeta.cpp
    writeHTMLQtPaintDevice.cpp
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

fast_configure_file(SGIItemQt ${SGI_INCLUDE_DIR}/sgi/plugins/SGIItemQt COPY_ONLY)

IF((UNIX AND FAST_PLATFORM_BITS EQUAL 32) OR WIN32 OR APPLE)
    LIST(APPEND TARGET_LIBRARIES_VARS OPENTHREADS_LIBRARY)
ENDIF()

LIST(APPEND TARGET_LIBRARIES_VARS
    QT_QTCORE_LIBRARY
    QT_QTGUI_LIBRARY
    QT_QTWIDGETS_LIBRARY
    )
IF ( SGI_PLUGIN_QT_WITH_QTOPENGL )
    LIST(APPEND TARGET_LIBRARIES_VARS QT_QTOPENGL_LIBRARY )
ENDIF()

SGI_PLUGIN(qt)
