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
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sgi/Export.in ${CMAKE_CURRENT_SOURCE_DIR}/sgi/Export)

FUNCTION(TO_LIST_NEWLINES _LIST_NAME OUTPUT_VAR)
  set(NEW_LIST_NEWLINES)
  foreach(ITEM ${${_LIST_NAME}})
    set(NEW_LIST_NEWLINES "${NEW_LIST_NEWLINES}\n${ITEM}")
  endforeach()
  set(${OUTPUT_VAR} "${NEW_LIST_NEWLINES}" PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(TO_LIST_COMMA _LIST_NAME OUTPUT_VAR)
  set(NEW_LIST_SPACE)
  foreach(ITEM ${${_LIST_NAME}})
    if(NEW_LIST_SPACE)
        set(NEW_LIST_SPACE "${NEW_LIST_SPACE}, ${ITEM}")
    else()
        set(NEW_LIST_SPACE "${ITEM}")
    endif()
  endforeach()
  string(STRIP ${NEW_LIST_SPACE} NEW_LIST_SPACE)
  set(${OUTPUT_VAR} "${NEW_LIST_SPACE}" PARENT_SCOPE)
ENDFUNCTION()

MACRO(SGI_PLUGIN_HOST_SUPPORT name)
    string(TOUPPER ${name} _var_name)
    IF(SGI_PLUGIN_${_var_name})
        string(TOLOWER ${name} _support_name)
        LIST(APPEND SGI_PLUGIN_HOST_SUPPORT_INCLUDES "#include \"${_support_name}_support.h\"")
        LIST(APPEND SGI_PLUGIN_HOST_SUPPORT_ITEM_CLASSES ${ARGN})
    ENDIF()
ENDMACRO(SGI_PLUGIN_HOST_SUPPORT)

set(SGI_PLUGIN_HOST_SUPPORT_INCLUDES)
set(SGI_PLUGIN_HOST_SUPPORT_ITEM_CLASSES)
set(SGI_PLUGIN_FALLBACK ON)
SGI_PLUGIN_HOST_SUPPORT(osg Osg)
SGI_PLUGIN_HOST_SUPPORT(osgearth OsgEarthConfig OsgEarthConfigOptions)
SGI_PLUGIN_HOST_SUPPORT(log4cplus Log4cplus)
SGI_PLUGIN_HOST_SUPPORT(qt Qt QtMeta)
SGI_PLUGIN_HOST_SUPPORT(fallback Fallback)

TO_LIST_NEWLINES(SGI_PLUGIN_HOST_SUPPORT_INCLUDES SGI_PLUGIN_HOST_SUPPORT_INCLUDES)
TO_LIST_COMMA(SGI_PLUGIN_HOST_SUPPORT_ITEM_CLASSES SGI_PLUGIN_HOST_SUPPORT_ITEM_CLASSES)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sgi/plugins/SGIHostSupport.in ${CMAKE_CURRENT_SOURCE_DIR}/sgi/plugins/SGIHostSupport)

SET(TARGET_H
	# precompiled header
	stdafx.h
	SGIPlugin.h
	entrypoint.h
	qt_helper.h
	sgi_internal.h
)

SET(TARGET_PUBLIC_H
    sgi/Export
    sgi/AutoLoadOsg
    sgi/AutoLoadQt
    sgi/WritePrettyHTML
    sgi/ObjectInfo
    sgi/ContextMenu
    sgi/SceneGraphDialog
    sgi/ObjectLoggerDialog

    sgi/plugins/fallback_support.h
    sgi/plugins/log4cplus_support.h
    sgi/plugins/osg_support.h
    sgi/plugins/osgearth_support.h
    sgi/plugins/qt_support.h

    sgi/plugins/SGIHostItemBase.h
    sgi/plugins/SGIHostItemFallback.h
    sgi/plugins/SGIHostItemLog4cplus.h
    sgi/plugins/SGIHostItemOsg.h
    sgi/plugins/SGIHostItemOsgEarth.h
    sgi/plugins/SGIHostItemQt.h

    sgi/plugins/SGIItemBase.h
    sgi/plugins/SGIPluginImpl.h
    sgi/plugins/SGIPluginInterface.h
    sgi/plugins/SGIPluginMacros.h

    sgi/plugins/SGIDataItemBase.h

    sgi/plugins/SGIHostItemGenerator
    sgi/plugins/SGIHostSupport
	)

SET(LIB_QT_UIS
	SceneGraphDialog.ui
	ObjectLoggerDialog.ui
	QTextDialog.ui
	)

SET(LIB_MOC_HDRS
	SceneGraphDialog.h
	ContextMenu.h
	ObjectLoggerDialog.h
	QtProxy.h
	QTextDialog.h
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
	SGIItemBase.cpp
	QtProxy.cpp
	QTextDialog.cpp
)

LIST(APPEND TARGET_SRC ${LIB_UI_SRCS} ${LIB_MOC_SRCS})


SET(TARGET_LIBRARIES_VARS
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
