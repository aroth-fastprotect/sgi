include_directories(${OSGEARTH_INCLUDE_DIR})
SET(TARGET_SRC
    stdafx.cpp
    sgi_osgearth_plugin.cpp
    writeHTMLOSGEarth.cpp
    writeHTMLOSGEarthConfig.cpp
    getObjectInfoOSGEarth.cpp
    ObjectTreeOSGEarth.cpp
    ContextMenuOSGEarth.cpp
    MenuActionOSGEarth.cpp
    string_helpers.cpp
    SettingsDialogOSGEarth.cpp
    GUIAdapterOSGEarth.cpp
    AutoClipPlaneCullCallbackDialog.cpp
    TileInspectorDialog.cpp
    RetrieveElevationDialog.cpp
	LODScaleOverrideDialog.cpp
    ShaderEditorDialog.cpp
    ShaderFunctionDetails.cpp
    geo_helpers.cpp
	osgearth_accessor.cpp
    )

SET(TARGET_H
	SGIItemOsgEarth
	stdafx.h
    writeHTMLOSGEarth.h
    writeHTMLOSGEarthConfig.h
    getObjectInfoOSGEarth.h
    ObjectTreeOSGEarth.h
    ContextMenuOSGEarth.h
    MenuActionOSGEarth.h
    SettingsDialogOSGEarth.h
    GUIAdapterOSGEarth.h
    string_helpers.h
    geo_helpers.h
	osgearth_accessor.h
    )

if(EXISTS ${OSGEARTH_INCLUDE_DIR}/osgEarthDBFactory/LevelDBFactory)
    add_definitions(-DSGI_USE_OSGEARTH_FAST=1)
endif()

SGI_CREATE_SYMLINK(SGIItemOsgEarth ${SGI_INCLUDE_DIR}/sgi/plugins/SGIItemOsgEarth)

SET(TARGET_MOC_H
    AutoClipPlaneCullCallbackDialog.h
    TileInspectorDialog.h
    RetrieveElevationDialog.h
	LODScaleOverrideDialog.h
    ShaderEditorDialog.h
    ShaderFunctionDetails.h
    )
SET(TARGET_UI
    AutoClipPlaneCullCallbackDialog.ui
    TileInspectorDialog.ui
    RetrieveElevationDialog.ui
	LODScaleOverrideDialog.ui
    ShaderEditorDialog.ui
    ShaderFunctionDetails.ui
    )

SET(TARGET_LIBRARIES_VARS
    SGI_OSG_HELPER_LIBRARY
    SGI_QT_HELPER_LIBRARY
    OPENTHREADS_LIBRARY
    OSG_LIBRARY
    OSGDB_LIBRARY
	OSGGA_LIBRARY
	OSGUTIL_LIBRARY
	OSGVIEWER_LIBRARY
    OSGEARTH_LIBRARY
    OSGEARTHUTIL_LIBRARY
	#OSGEARTHQT_LIBRARY
    OSGEARTHFEATURES_LIBRARY
    OSGEARTHANNOTATION_LIBRARY
	OSGEARTHSYMBOLOGY_LIBRARY
)
LIST(APPEND TARGET_LIBRARIES_VARS
    QT_QTCORE_LIBRARY
    QT_QTGUI_LIBRARY
    )
IF ( Qt5Widgets_FOUND )
	LIST(APPEND TARGET_LIBRARIES_VARS QT_QTWIDGETS_LIBRARY )
ENDIF()
if(FAST_PLATFORM)
    LIST(APPEND TARGET_DEFINITIONS OSGEARTH_WITH_FAST_MODIFICATIONS)
endif()
SGI_PLUGIN(osgearth)
