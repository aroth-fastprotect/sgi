
FAST_SET_TARGET_AND_PRODUCT_VERSION(SGI)
LIST(APPEND TARGET_H
    stdafx.h
    ${SGI_INCLUDE_DIR}/sgi/details/Object
    ${SGI_INCLUDE_DIR}/sgi/details/Observer
    ${SGI_INCLUDE_DIR}/sgi/details/Referenced
    ${SGI_INCLUDE_DIR}/sgi/details/UserDataContainer
    ${SGI_INCLUDE_DIR}/sgi/details/ref_ptr
    ${SGI_INCLUDE_DIR}/sgi/details/observer_ptr
)
SET(TARGET_SRC
    #stdafx.cpp
    Object.cpp
    Observer.cpp
    Referenced.cpp
    UserDataContainer.cpp
    )

FAST_STATIC_LIBRARY(sgi-ref NO_MOC_INCLUDE)
