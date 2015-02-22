SET(TARGET_SRC
    SGIItemBase.cpp
    ObjectTreeImpl.cpp
    osg_helpers.cpp
    qt_helpers.cpp
    string_helpers.cpp
    )

ADD_LIBRARY(osgSGI-plugin-impl STATIC ${TARGET_SRC})
