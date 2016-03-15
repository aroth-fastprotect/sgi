INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
IF(OSGEARTH_FOUND)
	include_directories(${OSGEARTH_INCLUDE_DIR})
ENDIF()

SET(CMAKE_SHARED_MODULE_PREFIX ${SGI_PLUGIN_PREFIX})

SET(LIB_NAME osgSGI)


SET(SGI_VERSION ${SGI_MAJOR_VERSION}.${SGI_MINOR_VERSION}.${SGI_PATCH_VERSION})

SET(TARGET_MAJOR_VERSION ${SGI_MAJOR_VERSION})
SET(TARGET_MINOR_VERSION ${SGI_MINOR_VERSION})
SET(TARGET_PATCH_VERSION ${SGI_PATCH_VERSION})
SET(TARGET_NO_TRANSLATION 1)
SET(TARGET_NO_IMPORT_LIB)

if(WIN32)
    set(_lib_prefix "")
else()
    set(_lib_prefix "lib")
endif()
SET(SGI_LIBRARY_NAME_DEBUG "${_lib_prefix}${LIB_NAME}${CMAKE_DEBUG_POSTFIX}${CMAKE_SHARED_MODULE_SUFFIX}")
SET(SGI_LIBRARY_NAME_RELEASE "${_lib_prefix}${LIB_NAME}${CMAKE_RELEASE_POSTFIX}${CMAKE_SHARED_MODULE_SUFFIX}")
SET(SGI_LIBRARY_NAME_RELWITHDEBINFO "${_lib_prefix}${LIB_NAME}${CMAKE_RELWITHDEBINFO_POSTFIX}${CMAKE_SHARED_MODULE_SUFFIX}")
SET(SGI_LIBRARY_NAME_MINSIZEREL "${_lib_prefix}${LIB_NAME}${CMAKE_MINSIZEREL_POSTFIX}${CMAKE_SHARED_MODULE_SUFFIX}")
fast_configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../../Export.in ${SGI_INCLUDE_DIR}/sgi/Export)

SET(TARGET_H
	# precompiled header
	stdafx.h
	SGIPlugin.h
	entrypoint.h
	ReferencedPicker.h
	sgi_internal.h
)

LIST(APPEND TARGET_H
    ${SGI_INCLUDE_DIR}/sgi/Export
    ${SGI_INCLUDE_DIR}/sgi/AutoLoadOsg
    ${SGI_INCLUDE_DIR}/sgi/AutoLoadQt
    ${SGI_INCLUDE_DIR}/sgi/WritePrettyHTML
    ${SGI_INCLUDE_DIR}/sgi/ObjectInfo
    ${SGI_INCLUDE_DIR}/sgi/ContextMenu
    ${SGI_INCLUDE_DIR}/sgi/SceneGraphDialog
    ${SGI_INCLUDE_DIR}/sgi/ObjectLoggerDialog
    ${SGI_INCLUDE_DIR}/sgi/ImagePreviewDialog

	${SGI_INCLUDE_DIR}/sgi/helpers/qt
	${SGI_INCLUDE_DIR}/sgi/helpers/qt_widgets
	${SGI_INCLUDE_DIR}/sgi/helpers/osg

	${SGI_INCLUDE_DIR}/sgi/details/type_list
	${SGI_INCLUDE_DIR}/sgi/details/functionImplBase

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

    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIDataItemBase.h

    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIHostItemGenerator
    ${SGI_INCLUDE_DIR}/sgi/plugins/SGIHostSupport

    ${SGI_INCLUDE_DIR}/sgi/plugins/ContextMenu
    ${SGI_INCLUDE_DIR}/sgi/plugins/SceneGraphDialog
    ${SGI_INCLUDE_DIR}/sgi/plugins/ObjectLoggerDialog
    ${SGI_INCLUDE_DIR}/sgi/plugins/ImagePreviewDialog

    ${SGI_INCLUDE_DIR}/sgi/plugins/SettingsDialogImpl
    ${SGI_INCLUDE_DIR}/sgi/plugins/SGISettingsDialogImpl
    ${SGI_INCLUDE_DIR}/sgi/plugins/WritePrettyHTMLImpl

	)

SET(TARGET_UI
	SceneGraphDialog.ui
	ObjectLoggerDialog.ui
	QTextDialog.ui
	DoubleInputDialog.ui
    ImagePreviewDialog.ui
    )

SET(TARGET_MOC_H
	SceneGraphDialog.h
	ContextMenu.h
	ObjectLoggerDialog.h
	QtProxy.h
	QTextDialog.h
	DoubleInputDialog.h
	ImagePreviewDialog.h
)

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
	ImagePreviewDialog.cpp
)

SET(TARGET_LIBRARIES_VARS
    SGI_PLUGIN_IMPL_LIBRARY
	OPENTHREADS_LIBRARY
	OSG_LIBRARY
	OSGDB_LIBRARY
	OSGUTIL_LIBRARY
	OSGGA_LIBRARY
	OSGVIEWER_LIBRARY
)
LIST(APPEND TARGET_LIBRARIES_VARS
    QT_QTCORE_LIBRARY
    QT_QTGUI_LIBRARY
    )
IF ( Qt5Widgets_FOUND )
	LIST(APPEND TARGET_LIBRARIES_VARS QT_QTWIDGETS_LIBRARY )
ENDIF()

FAST_SHARED_LIBRARY(${LIB_NAME})

target_compile_definitions(${TARGET_DEFAULT_PREFIX}${LIB_NAME} PRIVATE
    SGI_LIBRARY
    SGI_PLUGIN_EXTENSION=${CMAKE_SHARED_MODULE_SUFFIX}
    OSG_LIBRARY_POSTFIX="$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>$<$<CONFIG:Release>:${CMAKE_RELEASE_POSTFIX}>$<$<CONFIG:RelWithDebInfo>:${CMAKE_RELWITHDEBINFO_POSTFIX}>$<$<CONFIG:MinSizeRel>:${CMAKE_MINSIZEREL_POSTFIX}>"
    )
