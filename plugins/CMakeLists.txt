if(SGI_PLUGIN_OSG)
    add_subdirectory(osg)
endif()

if(SGI_PLUGIN_OSGEARTH)
    add_subdirectory(osgearth)
endif()

if(SGI_PLUGIN_QT)
    add_subdirectory(qt)
endif()

if(SGI_PLUGIN_QTQUICK)
    add_subdirectory(qtquick)
endif()

if(SGI_PLUGIN_LOG4CPLUS)
    add_subdirectory(log4cplus)
endif()

if(SGI_PLUGIN_VSG)
    add_subdirectory(vsg)
endif()
