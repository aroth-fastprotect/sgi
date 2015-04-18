IF(OSGEARTH_FOUND)
	include_directories(${OSGEARTH_INCLUDE_DIR})
ENDIF()
SET(TARGET_SRC
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

    getObjectInfoOSG.cpp

    ObjectTreeOSG.cpp
    ContextMenuOSG.cpp
    MenuActionOSG.cpp

    ObjectLoggerOSG.cpp
    GUIAdapterOSG.cpp

    stateset_helpers.cpp
    std_ostream_osgtext.cpp
    DrawableHelper.cpp

    SettingsDialogOSG.cpp
    CameraSettings.cpp
    ImagePreviewDialog.cpp
    )

SET(TARGET_H
    SGIItemOsg
    stdafx.h
    writeHTMLOSG.h
    writeHTMLOSGAnimation.h
    writeHTMLOSGDB.h
    writeHTMLOSGFX.h
    writeHTMLOSGGA.h
    writeHTMLOSGText.h
    writeHTMLOSGTerrain.h
    writeHTMLOSGUtil.h
    writeHTMLOSGViewer.h

    getObjectInfoOSG.h

    osg_accessor.h
    osgdb_accessor.h
    stateset_helpers.h
    std_ostream_osgtext.h

    ObjectLoggerOSG.h

    ObjectTreeOSG.h
    ContextMenuOSG.h
    MenuActionOSG.h
    SettingsDialogOSG.h
    GUIAdapterOSG.h

    FindTopMostNodeOfType
    DrawableHelper.h
    ManipulateObject.h
    )
configure_file(SGIItemOsg ${SGI_INCLUDE_DIR}/sgi/plugins/SGIItemOsg COPYONLY)

SET(TARGET_MOC_H
    CameraSettings.h
    ImagePreviewDialog.h
    )
SET(TARGET_UI
    CameraSettings.ui
    ImagePreviewDialog.ui
    )

SET(TARGET_LIBRARIES_VARS
    OPENTHREADS_LIBRARY
    OPENGL_LIBRARIES
    OSG_LIBRARY
    OSGANIMATION_LIBRARY
    OSGDB_LIBRARY
    OSGFX_LIBRARY
    OSGGA_LIBRARY
    OSGTEXT_LIBRARY
    OSGTERRAIN_LIBRARY
    OSGVIEWER_LIBRARY
    OSGUTIL_LIBRARY
    OSGQT_LIBRARY
    SGI_PLUGIN_IMPL_LIBRARY
)
IF ( NOT Qt5Widgets_FOUND )
    LIST(APPEND TARGET_LIBRARIES_VARS QT_QTCORE_LIBRARY QT_QTGUI_LIBRARY QT_QTOPENGL_LIBRARY)
ENDIF(  )

SGI_PLUGIN(osg)

IF ( Qt5Widgets_FOUND )
    qt5_use_modules( osgdb_sgi_osg_plugin Gui Widgets OpenGL)
ENDIF( Qt5Widgets_FOUND )
