IF(OSGEARTH_FOUND)
    include_directories(${OSGEARTH_INCLUDE_DIR})
ENDIF()
if(OSGQT_FOUND)
    include_directories(${OSGQT_INCLUDE_DIR})
endif()

set(TARGET_SRC sgi_viewer_base.cpp)
set(TARGET_H sgi_viewer_base.h)

FAST_STATIC_LIBRARY(sgi_viewer_base)
