include_directories(.)
SET(TARGET_SRC
    stdafx.cpp
    sgi_osg_plugin.cpp
    writeHTMLOSG.cpp
    writeHTMLOSGAnimation.cpp
    writeHTMLOSGDB.cpp
    writeHTMLOSGFX.cpp
    writeHTMLOSGGA.cpp
    writeHTMLOSGText.cpp
    writeHTMLOSGTerrain.cpp
    writeHTMLOSGUtil.cpp
    writeHTMLOSGViewer.cpp
	writeHTMLOSGCullingInfo.cpp

    getObjectInfoOSG.cpp

	osg_accessor.cpp

    ObjectTreeOSG.cpp
    ContextMenuOSG.cpp
    MenuActionOSG.cpp

    ObjectLoggerOSG.cpp
    GUIAdapterOSG.cpp

    stateset_helpers.cpp
    std_ostream_osgtext.cpp
	NodeHelper.cpp

    SettingsDialogOSG.cpp
    CameraSettings.cpp
	ExtraViewDialog.cpp
    )

SET(TARGET_H
    SGIItemOsg
    stdafx.h
    writeHTMLOSG.h

    getObjectInfoOSG.h

    osg_accessor.h
    osgdb_accessor.h
	osgtext_accessor.h
	osganimation_accessor.h
	osgviewer_accessor.h
    stateset_helpers.h
    std_ostream_osgtext.h

    ObjectLoggerOSG.h

    ObjectTreeOSG.h
    ContextMenuOSG.h
    MenuActionOSG.h
    SettingsDialogOSG.h
    GUIAdapterOSG.h

	NodeHelper.h
    )
SGI_CREATE_SYMLINK(SGIItemOsg ${SGI_INCLUDE_DIR}/sgi/plugins/SGIItemOsg)

SET(TARGET_MOC_H
    CameraSettings.h
	ExtraViewDialog.h
    )
SET(TARGET_UI
    CameraSettings.ui
	ExtraViewDialog.ui
    )

SET(TARGET_LIBRARIES_VARS
    OPENTHREADS_LIBRARY
    OSG_LIBRARY
    OSGANIMATION_LIBRARY
    OSGDB_LIBRARY
    OSGFX_LIBRARY
    OSGGA_LIBRARY
    OSGTEXT_LIBRARY
    OSGTERRAIN_LIBRARY
    OSGVIEWER_LIBRARY
    OSGUTIL_LIBRARY
    SGI_OSG_HELPER_LIBRARY
)
if(OPENGL_opengl_LIBRARY)
    list(APPEND TARGET_LIBRARIES_VARS OPENGL_opengl_LIBRARY)
else()
    list(APPEND TARGET_LIBRARIES_VARS OPENGL_gl_LIBRARY)
endif()

if(OSGQT_FOUND)
    include_directories(${OSGQT_INCLUDE_DIR})
    LIST(APPEND TARGET_LIBRARIES_VARS
        OSGQT_LIBRARY
    )
endif()
IF(OSGEARTH_FOUND)
	include_directories(${OSGEARTH_INCLUDE_DIR})
    LIST(APPEND TARGET_LIBRARIES_VARS
        OSGEARTH_LIBRARY
    )
ENDIF()

LIST(APPEND TARGET_LIBRARIES_VARS
    QT_QTCORE_LIBRARY
	QT_QTOPENGL_LIBRARY
    QT_QTGUI_LIBRARY
    )
IF ( Qt5Widgets_FOUND )
	LIST(APPEND TARGET_LIBRARIES_VARS QT_QTWIDGETS_LIBRARY )
ENDIF()

SGI_PLUGIN(osg)
