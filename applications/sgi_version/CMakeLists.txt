SET( TARGET_SRC
    main.cpp
)

LIST(APPEND TARGET_LIBRARIES_VARS
    QT_QTCORE_LIBRARY
    )

if(FAST_SGI_CMAKE_SYSTEM)
    file(RELATIVE_PATH _qt_plugin_dir ${FAST_WORK_BIN_DIR} ${FAST_WORK_PLUGINS_DIR})
    add_definitions(-DSGI_QT_PLUGIN_DIR="/${_qt_plugin_dir}")
else()
    VAR_CONFIG(_postfix FAST_POSTFIX)
    add_definitions(-DSGI_QT_PLUGIN_DIR="/../plugins${_postfix}")
endif()

configure_file(sgi_version.h.in ${CMAKE_CURRENT_BINARY_DIR}/sgi_version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

FAST_SET_TARGET_AND_PRODUCT_VERSION(SGI)
SET(TARGET_NO_TRANSLATION 1)

if(SGI_PLUGIN_OSG)
    list(APPEND TARGET_LIBRARIES_VARS OSG_LIBRARY)
endif()

if(SGI_PLUGIN_OSGEARTH)
    include_directories(${OSGEARTH_INCLUDE_DIR})
    list(APPEND TARGET_LIBRARIES_VARS OSGEARTH_LIBRARY)
endif()


FAST_COMMANDLINE_APPLICATION(sgi_version NO_MOC_INCLUDE )

