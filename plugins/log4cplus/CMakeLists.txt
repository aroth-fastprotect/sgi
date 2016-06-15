include_directories(${LOG4CPLUS_INCLUDE_DIR})

if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-Wno-deprecated-declarations)
endif()

SET(TARGET_SRC
    stdafx.cpp
    sgi_log4cplus_plugin.cpp
    ObjectTreeLog4cplus.cpp
    writeHTMLLog4cplus.cpp
    getObjectInfoLog4cplus.cpp
    ContextMenuLog4cplus.cpp
    MenuActionLog4cplus.cpp
    )

SET(TARGET_H
    stdafx.h
    SGIItemLog4cplus
    ObjectTreeLog4cplus.h
    writeHTMLLog4cplus.h
    getObjectInfoLog4cplus.h
    ContextMenuLog4cplus.h
    MenuActionLog4cplus.h
    )
fast_configure_file(SGIItemLog4cplus ${SGI_INCLUDE_DIR}/sgi/plugins/SGIItemLog4cplus COPY_ONLY)

SET(TARGET_LIBRARIES_VARS
    OSG_LIBRARY
    OSGDB_LIBRARY
    LOG4CPLUS_LIBRARY
)

IF((UNIX AND FAST_PLATFORM_BITS EQUAL 32) OR WIN32)
    LIST(APPEND TARGET_LIBRARIES_VARS OPENTHREADS_LIBRARY)
ENDIF()

LIST(APPEND TARGET_LIBRARIES_VARS
    QT_QTCORE_LIBRARY
    QT_QTGUI_LIBRARY
    )

SGI_PLUGIN(log4cplus)
