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
	ElevationQueryReferenced
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

fast_configure_file(SGIItemOsgEarth ${SGI_INCLUDE_DIR}/sgi/plugins/SGIItemOsgEarth COPY_ONLY)
fast_configure_file(ElevationQueryReferenced ${SGI_INCLUDE_DIR}/sgi/plugins/ElevationQueryReferenced COPY_ONLY)

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
	OSGGA_LIBRARY
	OSGUTIL_LIBRARY
	OSGVIEWER_LIBRARY
    OSGEARTH_LIBRARY
    OSGEARTHUTIL_LIBRARY
	OSGEARTHQT_LIBRARY
    OSGEARTHFEATURES_LIBRARY
    OSGEARTHANNOTATION_LIBRARY
	OSGEARTHSYMBOLOGY_LIBRARY
)
LIST(APPEND TARGET_LIBRARIES_VARS
    QT_QTCORE_LIBRARY
    QT_QTGUI_LIBRARY
	QT_QTOPENGL_LIBRARY
    )
IF ( Qt5Widgets_FOUND )
	LIST(APPEND TARGET_LIBRARIES_VARS QT_QTWIDGETS_LIBRARY )
ENDIF()
LIST(APPEND TARGET_DEFINITIONS OSGEARTH_WITH_FAST_MODIFICATIONS)
SGI_PLUGIN(osgearth NO_MOC_INCLUDE)
