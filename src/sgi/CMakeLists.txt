INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

SET(CMAKE_SHARED_MODULE_PREFIX ${SGI_PLUGIN_PREFIX})

SET(LIB_NAME osgSGI)


SET(SGI_VERSION ${SGI_MAJOR_VERSION}.${SGI_MINOR_VERSION}.${SGI_PATCH_VERSION})

SET(TARGET_MAJOR_VERSION ${SGI_MAJOR_VERSION})
SET(TARGET_MINOR_VERSION ${SGI_MINOR_VERSION})
SET(TARGET_PATCH_VERSION ${SGI_PATCH_VERSION})

if(WIN32)
    set(_lib_prefix "")
else()
    set(_lib_prefix "lib")
endif()
SET(SGI_LIBRARY_NAME_DEBUG "${_lib_prefix}${LIB_NAME}${CMAKE_DEBUG_POSTFIX}${CMAKE_SHARED_MODULE_SUFFIX}")
SET(SGI_LIBRARY_NAME_RELEASE "${_lib_prefix}${LIB_NAME}${CMAKE_RELEASE_POSTFIX}${CMAKE_SHARED_MODULE_SUFFIX}")
SET(SGI_LIBRARY_NAME_RELWITHDEBINFO "${_lib_prefix}${LIB_NAME}${CMAKE_RELWITHDEBINFO_POSTFIX}${CMAKE_SHARED_MODULE_SUFFIX}")
SET(SGI_LIBRARY_NAME_MINSIZEREL "${_lib_prefix}${LIB_NAME}${CMAKE_MINSIZEREL_POSTFIX}${CMAKE_SHARED_MODULE_SUFFIX}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../../Export.in ${SGI_INCLUDE_DIR}/sgi/Export)

SET(TARGET_H
	# precompiled header
	stdafx.h
	SGIPlugin.h
	entrypoint.h
	sgi_internal.h
)

SET(TARGET_H
    ${SGI_INCLUDE_DIR}/sgi/Export
    ${SGI_INCLUDE_DIR}/sgi/AutoLoadOsg
    ${SGI_INCLUDE_DIR}/sgi/AutoLoadQt
    ${SGI_INCLUDE_DIR}/sgi/WritePrettyHTML
    ${SGI_INCLUDE_DIR}/sgi/ObjectInfo
    ${SGI_INCLUDE_DIR}/sgi/ContextMenu
    ${SGI_INCLUDE_DIR}/sgi/SceneGraphDialog
    ${SGI_INCLUDE_DIR}/sgi/ObjectLoggerDialog

	${SGI_INCLUDE_DIR}/sgi/helpers/qt
	${SGI_INCLUDE_DIR}/sgi/helpers/osg

    ${SGI_INCLUDE_DIR}/sgi/plugins/fallback_support.h
    ${SGI_INCLUDE_DIR}/sgi/plugins/log4cplus_support.h
    ${SGI_INCLUDE_DIR}/sgi/plugins/osg_support.h
    ${SGI_INCLUDE_DIR}/sgi/plugins/osgearth_support.h
    ${SGI_INCLUDE_DIR}/sgi/plugins/qt_support.h

    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIHostItemBase.h
    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIHostItemFallback.h
    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIHostItemLog4cplus.h
    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIHostItemOsg.h
    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIHostItemOsgEarth.h
    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIHostItemQt.h

    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIItemBase.h
    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIPluginImpl.h
    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIPluginInterface.h
    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIPluginMacros.h

    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIDataItemBase.h

    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIHostItemGenerator
    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIHostSupport
	)

SET(LIB_QT_UIS
	SceneGraphDialog.ui
	ObjectLoggerDialog.ui
	QTextDialog.ui
	DoubleInputDialog.ui
	)

SET(LIB_MOC_HDRS
	SceneGraphDialog.h
	ContextMenu.h
	ObjectLoggerDialog.h
	QtProxy.h
	QTextDialog.h
	DoubleInputDialog.h
)


IF(Qt5Widgets_FOUND)
    #QT5_ADD_RESOURCES( LIB_RC_SRCS ${LIB_QT_RCS} )
    QT5_WRAP_UI( LIB_UI_HDRS ${LIB_QT_UIS} )
    IF(Qt5Widgets_VERSION VERSION_LESS 5.2.0)
        QT5_WRAP_CPP( LIB_UI_SRCS ${LIB_UI_HDRS} OPTIONS "-f" )
        QT5_WRAP_CPP( LIB_MOC_SRCS ${LIB_MOC_HDRS} OPTIONS "-f" )
    ELSE()
        QT5_WRAP_CPP( LIB_UI_SRCS ${LIB_UI_HDRS} )
        QT5_WRAP_CPP( LIB_MOC_SRCS ${LIB_MOC_HDRS} )
    ENDIF()
ELSE()
    INCLUDE( ${QT_USE_FILE} )
    #QT4_ADD_RESOURCES( LIB_RC_SRCS ${LIB_QT_RCS} )
    QT4_WRAP_UI( LIB_UI_HDRS ${LIB_QT_UIS} )
    QT4_WRAP_CPP( LIB_UI_SRCS ${LIB_UI_HDRS} OPTIONS "-f" )
    QT4_WRAP_CPP( LIB_MOC_SRCS ${LIB_MOC_HDRS} OPTIONS "-f" )
ENDIF()

# CHECK-IT:
# add your source files here
SET(TARGET_SRC
	stdafx.cpp
	SceneGraphDialog.cpp
	ContextMenu.cpp
	ObjectLoggerDialog.cpp
	SGIPlugin.cpp
	entrypoint.cpp
	sgi_internal.cpp
	QtProxy.cpp
	QTextDialog.cpp
	DoubleInputDialog.cpp
)

LIST(APPEND TARGET_SRC ${LIB_UI_SRCS} ${LIB_MOC_SRCS})


SET(TARGET_LIBRARIES_VARS
    SGI_PLUGIN_IMPL_LIBRARY
	OPENTHREADS_LIBRARY
	OSG_LIBRARY
	OSGDB_LIBRARY
	OSGUTIL_LIBRARY
	OSGGA_LIBRARY
	OSGVIEWER_LIBRARY
)
IF ( NOT Qt5Widgets_FOUND )
    LIST(APPEND TARGET_LIBRARIES_VARS
        QT_QTCORE_LIBRARY
        QT_QTGUI_LIBRARY
        )
ENDIF()

ADD_LIBRARY( ${LIB_NAME} SHARED
    ${TARGET_H} ${TARGET_PUBLIC_H}
    ${TARGET_SRC}
)

LINK_WITH_VARIABLES(${LIB_NAME} ${TARGET_LIBRARIES_VARS})
LINK_CORELIB_DEFAULT(${LIB_NAME} ${CMAKE_THREAD_LIBS_INIT} ${MATH_LIBRARY})

SET_PROPERTY(TARGET ${TARGET_DEFAULT_PREFIX}${LIB_NAME} PROPERTY FOLDER "Libs")

target_compile_definitions(${TARGET_DEFAULT_PREFIX}${LIB_NAME} PRIVATE
    SGI_LIBRARY
    SGI_PLUGIN_EXTENSION=${CMAKE_SHARED_MODULE_SUFFIX}
    OSG_LIBRARY_POSTFIX="$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>$<$<CONFIG:Release>:${CMAKE_RELEASE_POSTFIX}>$<$<CONFIG:RelWithDebInfo>:${CMAKE_RELWITHDEBINFO_POSTFIX}>$<$<CONFIG:MinSizeRel>:${CMAKE_MINSIZEREL_POSTFIX}>"
    )

IF ( Qt5Widgets_FOUND )
    qt5_use_modules( ${TARGET_DEFAULT_PREFIX}${LIB_NAME} Gui Widgets)
ENDIF( Qt5Widgets_FOUND )

INCLUDE(ModuleInstall OPTIONAL)
