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

fast_configure_file(SGIItemQt ${SGI_INCLUDE_DIR}/sgi/plugins/SGIItemQt COPY_ONLY)

SET(TARGET_UI
)

SET(TARGET_LIBRARIES_VARS
)

IF((UNIX AND FAST_PLATFORM_BITS EQUAL 32) OR WIN32)
    LIST(APPEND TARGET_LIBRARIES_VARS OPENTHREADS_LIBRARY)
ENDIF()

LIST(APPEND TARGET_LIBRARIES_VARS
    QT_QTCORE_LIBRARY
	QT_QTOPENGL_LIBRARY
    QT_QTGUI_LIBRARY
    )
IF ( Qt5Widgets_FOUND )
	LIST(APPEND TARGET_LIBRARIES_VARS QT_QTWIDGETS_LIBRARY )
ENDIF()

SGI_PLUGIN(qt NO_MOC_INCLUDE)
