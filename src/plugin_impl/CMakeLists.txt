IF(OSGEARTH_FOUND)
	include_directories(${OSGEARTH_INCLUDE_DIR})
ENDIF()

SET(TARGET_SRC
    SGIItemBase.cpp
    ObjectTreeImpl.cpp
    osg_helpers.cpp
    qt_helpers.cpp
    string_helpers.cpp
    )

SET(LIB_MOC_HDRS
	ObjectTreeImplProxy.h
	)

IF(Qt5Widgets_FOUND)
    IF(Qt5Widgets_VERSION VERSION_LESS 5.2.0)
        QT5_WRAP_CPP( LIB_MOC_SRCS ${LIB_MOC_HDRS} OPTIONS "-f" )
    ELSE()
        QT5_WRAP_CPP( LIB_MOC_SRCS ${LIB_MOC_HDRS} )
    ENDIF()
ELSE()
    INCLUDE( ${QT_USE_FILE} )
    QT4_WRAP_CPP( LIB_MOC_SRCS ${LIB_MOC_HDRS} OPTIONS "-f" )
ENDIF()

ADD_LIBRARY(osgSGI-plugin-impl STATIC ${TARGET_SRC} ${LIB_MOC_SRCS})
