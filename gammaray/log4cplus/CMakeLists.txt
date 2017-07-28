find_package(Gammaray)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-Wno-deprecated-declarations)
endif()

include_directories(${GAMMARAY_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
# probe plugin
set(TARGET_SRC
    log4cplus_plugin.cpp
    log4cplus_model.cpp
)
LIST(APPEND TARGET_PRIVATE_LIBRARIES QT_QTCORE QT_QTGUI)
LIST(APPEND TARGET_ADDED_LIBRARIES
    GAMMARAY_CORE_LIBRARY
    GAMMARAY_KITEMMODELS_LIBRARY
    LOG4CPLUS_LIBRARY
    )
gammaray_add_plugin(gammaray_log4cplus JSON gammaray_log4cplus.json)

# ui plugin
if(GAMMARAY_BUILD_UI)
  set(TARGET_SRC
    log4cplus_widget.cpp
  )
  set(TARGET_UI
    log4cplus_widget.ui
  )
    LIST(APPEND TARGET_PRIVATE_LIBRARIES QT_QTCORE QT_QTGUI QT_QTWIDGETS)
    LIST(APPEND TARGET_ADDED_LIBRARIES GAMMARAY_CORE_LIBRARY GAMMARAY_UI_LIBRARY sgi_gammaray_ui)
  gammaray_add_plugin(gammaray_log4cplus_ui JSON gammaray_log4cplus.json)
endif()
