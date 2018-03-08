

set(TARGET_SRC
    deferredtreeview.cpp
    searchlinecontroller.cpp
    )
set(TARGET_MOC_H
    deferredtreeview.h
    deferredtreeview_p.h
    searchlinecontroller.h
    )
set(TARGET_DEFINITIONS HAVE_PRIVATE_QT_HEADERS)
FAST_STATIC_LIBRARY(sgi_gammaray_ui)
