include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Widgets_PRIVATE_INCLUDE_DIRS})


SET(TARGET_SRC
    stdafx.cpp
    sgi_qtquick_plugin.cpp
    writeHTMLQtQuick.cpp
    getObjectInfoQtQuick.cpp
    ObjectTreeQtQuick.cpp
    ContextMenuQtQuick.cpp
    MenuActionQtQuick.cpp
    SettingsDialogQtQuick.cpp
    )

SET(TARGET_H
    SGIItemQtQuick
    stdafx.h
    writeHTMLQtQuick.h
    getObjectInfoQtQuick.h
    ObjectTreeQtQuick.h
    ContextMenuQtQuick.h
    MenuActionQtQuick.h
    SettingsDialogQtQuick.h
    )

SGI_CREATE_SYMLINK(SGIItemQtQuick ${SGI_INCLUDE_DIR}/sgi/plugins/SGIItemQtQuick)

list(APPEND TARGET_INCLUDE_DIRS
    QT_QTQML_INCLUDE_DIR
    QT_QTQUICK_INCLUDE_DIR
    QT_QTQUICKWIDGETS_INCLUDE_DIR
    )

LIST(APPEND TARGET_LIBRARIES_VARS
    SGI_QT_HELPER_LIBRARY
    QT_QTCORE_LIBRARY
    QT_QTGUI_LIBRARY
    QT_QTWIDGETS_LIBRARY
    QT_QTQML_LIBRARY
    QT_QTQUICK_LIBRARY
    QT_QTQUICKWIDGETS_LIBRARY
    )

SGI_PLUGIN(qtquick)
