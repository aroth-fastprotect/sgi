include_directories(${VSG_INCLUDE_DIR})

if(CMAKE_COMPILER_IS_GNUCXX OR CLANG)
    add_compile_options(-Wno-deprecated-declarations)
endif()

SET(TARGET_SRC
    stdafx.cpp
    sgi_vsg_plugin.cpp
    ObjectTreeVsg.cpp
    writeHTMLVsg.cpp
    getObjectInfoVsg.cpp
    ContextMenuVsg.cpp
    MenuActionVsg.cpp
    )

SET(TARGET_H
    stdafx.h
    SGIItemVsg
    ObjectTreeVsg.h
    writeHTMLVsg.h
    getObjectInfoVsg.h
    ContextMenuVsg.h
    MenuActionVsg.h
    )
SGI_CREATE_SYMLINK(SGIItemVsg ${SGI_INCLUDE_DIR}/sgi/plugins/SGIItemVsg)

LIST(APPEND TARGET_LIBRARIES_VARS
    QT_QTCORE_LIBRARY
    QT_QTGUI_LIBRARY
    vsg::vsg
    )

SGI_PLUGIN(vsg)
