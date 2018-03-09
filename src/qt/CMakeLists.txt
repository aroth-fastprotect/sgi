if(SGI_USE_GAMMARAY)
    add_definitions(-DGAMMARAY_PLUGIN_VERSION=\"${GAMMARAY_PLUGIN_VERSION}\"
                    -DGAMMARAY_PLUGIN_INSTALL_DIR=\"${GAMMARAY_PLUGIN_INSTALL_DIR}\"
                    )
endif()

LIST(APPEND TARGET_SRC
        main.cpp
        )

LIST(APPEND TARGET_MOC_H
    main.h
    )

LIST(APPEND TARGET_QRC
        sgi_logo.qrc
        )

#OTHER_FILES += sgi_loader.json
LIST(APPEND TARGET_LIBRARIES_VARS
    QT_QTCORE_LIBRARY
    QT_QTGUI_LIBRARY
    )
IF ( Qt5Widgets_FOUND )
    # Add the include directories for the Qt 5 Widgets module to
    # the compile lines.
    include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Widgets_PRIVATE_INCLUDE_DIRS})
    LIST(APPEND TARGET_LIBRARIES_VARS QT_QTWIDGETS_LIBRARY )
ENDIF()

FAST_SET_TARGET_AND_PRODUCT_VERSION(SGI)
SET(TARGET_NO_TRANSLATION 1)

FAST_PLUGIN(qsgi_loader PLUGINTYPE imageformats NO_MOC_INCLUDE)

if(SGI_GENERATE_QT_CONF)
    if(NOT QT_QMAKE_EXECUTABLE)
        set(QT_QMAKE_EXECUTABLE qmake)
    endif()
    execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
                    OUTPUT_VARIABLE QT_INSTALL_PLUGINS
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    )
    fast_configure_file(qt.conf.in ${FAST_WORK_BIN_DIR}/qt.conf @ONLY)
else()
    file(REMOVE ${FAST_WORK_BIN_DIR}/qt.conf)
endif()
