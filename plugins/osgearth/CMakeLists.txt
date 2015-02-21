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
    geo_helpers.cpp
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
    elevquery_ref.h
    geo_helpers.h
    )

fast_configure_file(SGIItemOsgEarth ${SGI_INCLUDE_DIR}/sgi/plugins/SGIItemOsgEarth COPY_ONLY)

SET(TARGET_MOC_H
    AutoClipPlaneCullCallbackDialog.h
    TileInspectorDialog.h
    RetrieveElevationDialog.h
    )
SET(TARGET_UI
    AutoClipPlaneCullCallbackDialog.ui
    TileInspectorDialog.ui
    RetrieveElevationDialog.ui
    )

SET(TARGET_LIBRARIES_VARS
    OPENTHREADS_LIBRARY
    OSG_LIBRARY
    OSGDB_LIBRARY
    OSGEARTH_LIBRARY
    OSGEARTHUTIL_LIBRARY
    OSGEARTHFEATURES_LIBRARY
    OSGEARTHANNOTATION_LIBRARY
	OSGEARTHSYMBOLOGY_LIBRARY
)
IF ( NOT Qt5Widgets_FOUND )
    LIST(APPEND TARGET_LIBRARIES_VARS QT_QTCORE_LIBRARY QT_QTGUI_LIBRARY)
ENDIF(  )
SET(TARGET_ADDED_LIBRARIES osgSGI-plugin-impl)

SGI_PLUGIN(osgearth NO_MOC_INCLUDE)

IF ( Qt5Widgets_FOUND )
    qt5_use_modules( osgdb_sgi_osgearth_plugin Gui Widgets)
ENDIF( Qt5Widgets_FOUND )
