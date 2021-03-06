IF(OSGEARTH_FOUND)
	include_directories(${OSGEARTH_INCLUDE_DIR})
ENDIF()

# here we use the command to generate the library
#not sure if needed, but for plugins only Msvc need the d suffix
IF(MSVC)
    SET(TARGET_NO_DEBUG_POSTFIX)
ENDIF(MSVC)

SET(TARGET_LABEL_PREFIX "osg Plugin")
SET(TARGET_NO_IMPORT_LIB 1)
SET(TARGET_NO_MANIFEST 1)
SET(TARGET_NO_LIBPREFIX 1)

FAST_SET_TARGET_AND_PRODUCT_VERSION(SGI)
SET(TARGET_PRODUCT_NAME "SGI")
SET(TARGET_NO_TRANSLATION 1)

SET(LIB_NAME sgi_loader)

SET(_plugin_dir osgPlugins-${OPENSCENEGRAPH_VERSION})
IF(WIN32)
    SET(TARGET_BIN_DIR ${FAST_WORK_BIN_DIR}/${_plugin_dir})
    set(TARGET_LIB_DIR ${FAST_WORK_BIN_DIR}/${_plugin_dir})
ELSE(WIN32)
    SET(TARGET_BIN_DIR ${FAST_WORK_LIB_DIR}/${_plugin_dir})
    set(TARGET_LIB_DIR ${FAST_WORK_LIB_DIR}/${_plugin_dir})
ENDIF(WIN32)
SET(TARGET_INSTALL_RUNTIME_DIR ${_plugin_dir})
SET(TARGET_INSTALL_LIBRARY_DIR ${_plugin_dir})

SET(TARGET_SRC ReaderWriteSGI.cpp)

include_directories(${QT_QTOPENGL_INCLUDE_DIR})

SET(TARGET_LIBRARIES_VARS
    OPENTHREADS_LIBRARY
    OSG_LIBRARY
    OSGDB_LIBRARY
    OSGUTIL_LIBRARY
    OSGGA_LIBRARY
    OSGVIEWER_LIBRARY
    SGI_REF_LIBRARY
    SGI_PLUGIN_IMPL_LIBRARY
    SGI_OSG_HELPER_LIBRARY
)
if(OSGEARTH_FOUND)
    LIST(APPEND TARGET_LIBRARIES_VARS
        OSGEARTH_LIBRARY
    )
endif()
if(OSGQT_FOUND)
    include_directories(${OSGQT_INCLUDE_DIR})
    LIST(APPEND TARGET_LIBRARIES_VARS
        OSGQT_LIBRARY
    )
endif()
IF(UNIX AND NOT APPLE)
    LIST(APPEND TARGET_LIBRARIES_VARS rt )
ENDIF()
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

# CHECK-IT:
# choose the target type (lib or application)
FAST_MODULE_LIBRARY("osgdb_${LIB_NAME}")


