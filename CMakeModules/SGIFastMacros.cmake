macro(FAST_PROJECT prjname)
    if(COMMAND cmake_policy)
        # Works around warnings libraries linked against that don't
        # have absolute paths (e.g. -lpthreads)
        cmake_policy(SET CMP0003 NEW)

        # Works around warnings about escaped quotes in ADD_DEFINITIONS
        # statements.
        cmake_policy(SET CMP0005 NEW)

        # Qt5 qt5_use_modules usage was causing "Policy CMP0043 is not set: Ignore COMPILE_DEFINITIONS_<Config> properties." warnings
        cmake_policy(SET CMP0043 NEW)
        
        if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.10.0")
            # Let AUTOMOC and AUTOUIC process GENERATED files.
            cmake_policy(SET CMP0071 NEW)
        endif()
    endif(COMMAND cmake_policy)

    if(UNIX)
        # use, i.e. don't skip the full RPATH for the build tree
        SET(CMAKE_SKIP_BUILD_RPATH FALSE)

        # already use RPATH on building (not just when installed)
        SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

        #SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
        SET(CMAKE_INSTALL_RPATH "$ORIGIN")

        # add the automatically determined parts of the RPATH
        # which point to directories outside the build tree to the install RPATH
        SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

        SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
        SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

    endif()

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    #
    # Set up CMake to use Solution Folders in VS.
    #
    SET_PROPERTY( GLOBAL PROPERTY USE_FOLDERS               ON )
    SET_PROPERTY( GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets" )

    # Common to all platforms:
    SET(CMAKE_DEBUG_POSTFIX  "d" CACHE STRING "add a postfix, usually d on windows")
    SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
    SET(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "add a postfix, usually empty on windows")
    SET(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "add a postfix, usually empty on windows")

    project(${prjname} ${ARGN})

    # Add -O0 to remove optimizations when using gcc
    IF(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)

    set(FAST_WORK_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
    set(FAST_WORK_LIB_DIR ${CMAKE_BINARY_DIR}/lib)
    set(FAST_WORK_PLUGINS_DIR ${FAST_WORK_LIB_DIR}/plugins)
    set(FAST_SGI_CMAKE_SYSTEM 1)

    FAST_RESET_TARGET_PRIV()

endmacro(FAST_PROJECT)

macro(FAST_IS_SYSTEM_LIBRARY prjname var)
    set(${var} 0)
endmacro(FAST_IS_SYSTEM_LIBRARY)


macro(fast_package_qt5)

    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets OpenGL Test)

    set(QT_QTCORE_LIBRARY Qt5::Core )
    set(QT_QTGUI_LIBRARY Qt5::Gui)
    set(QT_QTWIDGETS_LIBRARY Qt5::Widgets )
    set(QT_QTOPENGL_LIBRARY Qt5::OpenGL )
    set(QT_QTTEST_LIBRARY Qt5::Test )
endmacro(fast_package_qt5)

macro(fast_package_osg)

	find_package(OSG)

	find_package(OsgEarth)
	find_package(OsgQt)

endmacro(fast_package_osg)

macro(fast_package_log4cplus)
    find_package(Log4cplus)
endmacro(fast_package_log4cplus)

macro(fast_configure_file)
    configure_file(${ARGN})
endmacro()

macro(FAST_INSTALL_EXTRA_HEADER_FILES)
	cmake_parse_arguments(_tmp
		"VERBOSE;INTERFACE;PRIVATE;PUBLIC"
		"DESTINATION;COMPONENT"
		"DIRECTORY"
		${ARGN})
    if(NOT _tmp_COMPONENT)
        set(_tmp_COMPONENT "Developer files")
    endif()
    if(_tmp_DIRECTORY)
        install(DIRECTORY ${_tmp_DIRECTORY} DESTINATION ${_tmp_DESTINATION} COMPONENT ${_tmp_COMPONENT})
    endif()
endmacro(FAST_INSTALL_EXTRA_HEADER_FILES)

macro(FAST_INSTALL_DATA_FILE)
	cmake_parse_arguments(_tmp
		"VERBOSE;INTERFACE;PRIVATE;PUBLIC"
		"DESTINATION;COMPONENT"
		"DIRECTORY;FILES"
		${ARGN})
    if(_tmp_DIRECTORY)
        install(DIRECTORY ${_tmp_DIRECTORY} DESTINATION ${_tmp_DESTINATION} COMPONENT ${_tmp_COMPONENT})
    endif()
    if(_tmp_FILES)
        install(FILES ${_tmp_FILES} DESTINATION ${_tmp_DESTINATION} COMPONENT ${_tmp_COMPONENT})
    endif()
endmacro(FAST_INSTALL_DATA_FILE)

macro(FAST_PROJECT_PACKAGE)
endmacro(FAST_PROJECT_PACKAGE)

macro(FAST_RESET_TARGET_PRIV)
    set(TARGET_TARGETNAME)
    set(TARGET_LABEL)
    set(TARGET_SRC)
    set(TARGET_SRC_NO_PCH)
    set(TARGET_QRC)
    set(TARGET_H)
    set(TARGET_MOC_H)
    set(TARGET_GLSL)
    set(TARGET_IN)
    set(TARGET_DEFINITIONS)
    set(TARGET_DEFINITIONS_DEBUG "_DEBUG")
    set(TARGET_DEFINITIONS_RELEASE "NDEBUG")
    set(TARGET_DEFINITIONS_RELWITHDEBINFO "NDEBUG;_RELWITHDEBINFO")
    set(TARGET_DEFINITIONS_MINSIZEREL "NDEBUG;_MINSIZEREL")

endmacro(FAST_RESET_TARGET_PRIV)

FUNCTION(SET_IF_UNSET VAR DEFAULT_VALUE)
	if(NOT ${VAR})
		set(${VAR} ${DEFAULT_VALUE} PARENT_SCOPE)
	endif()
ENDFUNCTION()

FUNCTION(VAR_CONFIG RETVAL VAR)
	set(${RETVAL} "$<$<CONFIG:Debug>:${${VAR}_DEBUG}>$<$<CONFIG:Release>:${${VAR}_RELEASE}>$<$<CONFIG:RelWithDebInfo>:${${VAR}_RELWITHDEBINFO}>$<$<CONFIG:MinSizeRel>:${${VAR}_MINSIZEREL}>" PARENT_SCOPE)
ENDFUNCTION()


FUNCTION(FAST_SET_TARGET_AND_PRODUCT_VERSION SRC)
	SET(TARGET_SOVERSION ${${SRC}_SOVERSION} PARENT_SCOPE)
	SET(TARGET_MAJOR_VERSION ${${SRC}_VERSION_MAJOR} PARENT_SCOPE)
	SET(TARGET_MINOR_VERSION ${${SRC}_VERSION_MINOR} PARENT_SCOPE)
	SET(TARGET_PATCH_VERSION ${${SRC}_VERSION_PATCH} PARENT_SCOPE)
	SET(TARGET_BUILD_VERSION ${${SRC}_VERSION_BUILD} PARENT_SCOPE)
	SET(TARGET_PRODUCT_MAJOR_VERSION ${${SRC}_VERSION_MAJOR} PARENT_SCOPE)
	SET(TARGET_PRODUCT_MINOR_VERSION ${${SRC}_VERSION_MINOR} PARENT_SCOPE)
	SET(TARGET_PRODUCT_PATCH_VERSION ${${SRC}_VERSION_PATCH} PARENT_SCOPE)
	SET(TARGET_PRODUCT_BUILD_VERSION ${${SRC}_VERSION_BUILD} PARENT_SCOPE)
	SET(TARGET_PRODUCT_COPYRIGHT ${${SRC}_COPYRIGHT_TEXT} PARENT_SCOPE)
ENDFUNCTION()

# @macro FAST_LINK_LIBRARIES_PRIV
# @internal
# @brief does the magic and calls TARGET_LINK_LIBRARIES
# @note this macro is for internal use only. Do not use it in projects.
# @detail used by FAST_SETUP_LINK_LIBRARIES_PRIV. It automatically selects the
#         best available library (release or debug).
# @param TRGTNAME target name
# @param ARGV list of libraries (variables or direct library filenames)
MACRO(FAST_LINK_LIBRARIES_PRIV TRGTNAME)

	cmake_parse_arguments(FAST_LINK_LIBRARIES_PRIV_LOCAL
		"VERBOSE;INTERFACE;PRIVATE;PUBLIC"
		""
		"LIBRARIES"
		${ARGN})
	IF(FAST_LINK_LIBRARIES_PRIV_LOCAL_VERBOSE)
		SET(FAST_LINK_LIBRARIES_PRIV_LOCAL_VERBOSE "VERBOSE")
	ELSE(FAST_LINK_LIBRARIES_PRIV_LOCAL_VERBOSE)
		SET(FAST_LINK_LIBRARIES_PRIV_LOCAL_VERBOSE "")
	ENDIF()

	IF(FAST_LINK_LIBRARIES_PRIV_LOCAL_INTERFACE)
        SET(_link_opts "INTERFACE")
    ELSEIF(FAST_LINK_LIBRARIES_PRIV_LOCAL_PRIVATE)
        SET(_link_opts "PRIVATE")
    ELSEIF(FAST_LINK_LIBRARIES_PRIV_LOCAL_PUBLIC)
        SET(_link_opts "PUBLIC")
    ELSE()
        SET(_link_opts "PRIVATE")
    ENDIF()

		IF(FAST_LINK_LIBRARIES_PRIV_LOCAL_VERBOSE)
			MESSAGE(STATUS "FAST_LINK_LIBRARIES_PRIV ${TRGTNAME} in=${FAST_LINK_LIBRARIES_PRIV_LOCAL_LIBRARIES}")
		ENDIF()

	SET(_link_libs)
    FOREACH(varname ${FAST_LINK_LIBRARIES_PRIV_LOCAL_LIBRARIES})

		#MESSAGE(STATUS "varname=${varname}")
		IF(DEFINED ${varname})
			# got a variable name
			# check first if we have RELEASE and DEBUG variables too
			IF(${varname}_RELEASE AND ${varname}_DEBUG)
				LIST(APPEND _link_libs ${_link_opts} optimized "${${varname}_RELEASE}" debug "${${varname}_DEBUG}")
			ELSEIF(${varname} AND ${varname}_DEBUG)
				LIST(APPEND _link_libs ${_link_opts} optimized "${${varname}}" debug "${${varname}_DEBUG}")
			ELSEIF(${varname}_DEBUG)
				# Do not link a release against debug libs
				LIST(APPEND _link_libs ${_link_opts} debug "${${varname}_DEBUG}")
			ELSEIF(${varname}_RELEASE)
				# Use release libs for debug and release
				LIST(APPEND _link_libs ${_link_opts} general "${${varname}_RELEASE}")
			ELSE()
				LIST(APPEND _link_libs ${_link_opts} "${${varname}}")
			ENDIF()
        ELSEIF(DEFINED ${varname}_LIBRARY)
            # got a variable name
            # check first if we have RELEASE and DEBUG variables too
            IF(${varname}_LIBRARY_RELEASE AND ${varname}_LIBRARY_DEBUG)
                LIST(APPEND _link_libs ${_link_opts} optimized "${${varname}_LIBRARY_RELEASE}" debug "${${varname}_LIBRARY_DEBUG}")
            ELSEIF(${varname}_LIBRARY_DEBUG)
                # Do not link a release against debug libs
                LIST(APPEND _link_libs ${_link_opts} debug "${${varname}_LIBRARY_DEBUG}")
            ELSEIF(${varname}_LIBRARY_RELEASE)
                # Use release libs for debug and release
                LIST(APPEND _link_libs ${_link_opts} general "${${varname}_LIBRARY_RELEASE}")
            ELSE()
                LIST(APPEND _link_libs ${_link_opts} "${${varname}_LIBRARY}")
            ENDIF()
		ELSE()
			LIST(APPEND _link_libs ${_link_opts} "${varname}")
		ENDIF()
    ENDFOREACH(varname)
	IF(_link_libs)
		IF(FAST_LINK_LIBRARIES_PRIV_LOCAL_VERBOSE)
			MESSAGE(STATUS "FAST_LINK_LIBRARIES_PRIV ${TRGTNAME} ${_link_libs}")
		ENDIF()
		TARGET_LINK_LIBRARIES(${TRGTNAME} ${_link_libs})
	ENDIF()
ENDMACRO(FAST_LINK_LIBRARIES_PRIV)

MACRO(SETUP_LINK_LIBRARIES)
    FAST_LINK_LIBRARIES_PRIV(${TARGET_TARGETNAME} LIBRARIES ${TARGET_LIBRARIES_VARS} ${TARGET_ADDED_LIBRARIES} ${TARGET_EXTERNAL_LIBRARIES} ${FAST_SETUP_LINK_LIBRARIES_PRIV_LOCAL_VERBOSE})
    SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES LINK_INTERFACE_LIBRARIES "${TARGET_LINK_INTERFACE_LIBRARIES}")
ENDMACRO(SETUP_LINK_LIBRARIES)

MACRO(SETUP_OUTPUT_DIRS)

    SET_IF_UNSET(TARGET_BIN_DIR ${FAST_WORK_BIN_DIR})
    SET_IF_UNSET(TARGET_LIB_DIR ${FAST_WORK_LIB_DIR})

    IF(TARGET_NO_LIBPREFIX)
        # Remove the "lib" prefix since the library is already named "lib...".
        SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES PREFIX "")
    ENDIF()

    if(WIN32)
        FILE(MAKE_DIRECTORY "${TARGET_BIN_DIR}")
        SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TARGET_BIN_DIR})
        SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${TARGET_BIN_DIR})
    else()
        FILE(MAKE_DIRECTORY "${TARGET_LIB_DIR}")
        SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TARGET_BIN_DIR})
        SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${TARGET_LIB_DIR})
    endif()
    SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${TARGET_LIB_DIR})

    # Per-configuration property (VS, Xcode)
    FOREACH(CONF ${CMAKE_CONFIGURATION_TYPES})        # For each configuration (Debug, Release, MinSizeRel... and/or anything the user chooses)
        STRING(TOUPPER "${CONF}" CONF)                # Go uppercase (DEBUG, RELEASE...)

        # We use "FILE(TO_CMAKE_PATH", to create nice looking paths
        if(WIN32)
            SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES "ARCHIVE_OUTPUT_DIRECTORY_${CONF}" "${TARGET_BIN_DIR}")
            SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES "RUNTIME_OUTPUT_DIRECTORY_${CONF}" "${TARGET_BIN_DIR}")
        else()
            SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES "ARCHIVE_OUTPUT_DIRECTORY_${CONF}" "${TARGET_BIN_DIR}")
            SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES "RUNTIME_OUTPUT_DIRECTORY_${CONF}" "${TARGET_LIB_DIR}")
        endif()
        SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES "LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${TARGET_LIB_DIR}")
    ENDFOREACH(CONF ${CMAKE_CONFIGURATION_TYPES})
    
    IF(TARGET_VERSION)
		SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES VERSION ${TARGET_VERSION})
	ENDIF()
	IF(TARGET_TYPE STREQUAL "SHARED_LIBRARY" OR TARGET_TYPE STREQUAL "MODULE" OR TARGET_TYPE STREQUAL "MODULE_LIBRARY")
        if(TARGET_SOVERSION MATCHES "^[0-9]+$")
            #message("SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES SOVERSION ${TARGET_SOVERSION})")
            SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES SOVERSION ${TARGET_SOVERSION})
        ENDIF()
    ENDIF()

    target_compile_definitions(${TARGET_TARGETNAME}
                                PRIVATE
                                    ${TARGET_DEFINITIONS}
                                    $<$<CONFIG:Debug>:${TARGET_DEFINITIONS_DEBUG}>
                                    $<$<CONFIG:Release>:${TARGET_DEFINITIONS_RELEASE}>
                                    $<$<CONFIG:RelWithDebInfo>:${TARGET_DEFINITIONS_RELWITHDEBINFO}>
                                    $<$<CONFIG:MinSizeRel>:${TARGET_DEFINITIONS_MINSIZEREL}>
        )

ENDMACRO(SETUP_OUTPUT_DIRS)

MACRO(FAST_MODULE_LIBRARY PLUGIN_NAME)

    SET(TARGET_NAME ${PLUGIN_NAME} )

    #MESSAGE("in -->SETUP_PLUGIN<-- ${TARGET_NAME}-->${TARGET_SRC} <--> ${TARGET_H}<--")

    SOURCE_GROUP( "Header Files"   FILES ${TARGET_H} ${TARGET_MOC_H} )
    SOURCE_GROUP( "Shader Files"   FILES ${TARGET_GLSL} )
    SOURCe_GROUP( "Template Files" FILES ${TARGET_IN} )

    ## we have set up the target label and targetname by taking into account global prfix (osgdb_)

    IF(NOT TARGET_TARGETNAME)
            SET(TARGET_TARGETNAME "${TARGET_DEFAULT_PREFIX}${TARGET_NAME}")
    ENDIF(NOT TARGET_TARGETNAME)
    IF(NOT TARGET_LABEL)
            SET(TARGET_LABEL "${TARGET_DEFAULT_LABEL_PREFIX} ${TARGET_NAME}")
    ENDIF(NOT TARGET_LABEL)

# here we use the command to generate the library

    ADD_LIBRARY(${TARGET_TARGETNAME} MODULE ${TARGET_SRC} ${TARGET_SRC_NO_PCH} ${TARGET_QRC} ${TARGET_H} ${TARGET_MOC_H} ${TARGET_GLSL} ${TARGET_IN})

    SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "${TARGET_LABEL}")
    SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

    SETUP_LINK_LIBRARIES()
    SETUP_OUTPUT_DIRS()

#the installation path are differentiated for win32 that install in bib versus other architecture that install in lib${LIB_POSTFIX}/${VPB_PLUGINS}
    IF(WIN32)
        INSTALL(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION bin )

    ELSE(WIN32)
        INSTALL(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin ARCHIVE DESTINATION lib${LIB_POSTFIX} LIBRARY DESTINATION lib${LIB_POSTFIX} )
    ENDIF(WIN32)

#finally, set up the solution folder -gw
    SET_PROPERTY(TARGET ${TARGET_TARGETNAME} PROPERTY FOLDER "Plugins")

    FAST_RESET_TARGET_PRIV()

ENDMACRO(FAST_MODULE_LIBRARY)

macro(FAST_SHARED_LIBRARY PLUGIN_NAME)

    SET(TARGET_NAME ${PLUGIN_NAME} )

    #MESSAGE("in -->SETUP_PLUGIN<-- ${TARGET_NAME}-->${TARGET_SRC} <--> ${TARGET_H}<--")

    SOURCE_GROUP( "Header Files"   FILES ${TARGET_H} ${TARGET_MOC_H} )
    SOURCE_GROUP( "Shader Files"   FILES ${TARGET_GLSL} )
    SOURCe_GROUP( "Template Files" FILES ${TARGET_IN} )

    ## we have set up the target label and targetname by taking into account global prfix (osgdb_)

    IF(NOT TARGET_TARGETNAME)
            SET(TARGET_TARGETNAME "${TARGET_DEFAULT_PREFIX}${TARGET_NAME}")
    ENDIF(NOT TARGET_TARGETNAME)
    IF(NOT TARGET_LABEL)
            SET(TARGET_LABEL "${TARGET_DEFAULT_LABEL_PREFIX} ${TARGET_NAME}")
    ENDIF(NOT TARGET_LABEL)

# here we use the command to generate the library
    ADD_LIBRARY(${TARGET_TARGETNAME} SHARED ${TARGET_SRC} ${TARGET_SRC_NO_PCH} ${TARGET_QRC} ${TARGET_H} ${TARGET_MOC_H} ${TARGET_GLSL} ${TARGET_IN})

    SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "${TARGET_LABEL}")
    SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

    SETUP_LINK_LIBRARIES()
    SETUP_OUTPUT_DIRS()

#the installation path are differentiated for win32 that install in bib versus other architecture that install in lib${LIB_POSTFIX}/${VPB_PLUGINS}
    IF(WIN32)
        INSTALL(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION bin )

    ELSE(WIN32)
        INSTALL(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin ARCHIVE DESTINATION lib${LIB_POSTFIX} LIBRARY DESTINATION lib${LIB_POSTFIX} )
    ENDIF(WIN32)

#finally, set up the solution folder -gw
    SET_PROPERTY(TARGET ${TARGET_TARGETNAME} PROPERTY FOLDER "Plugins")

    FAST_RESET_TARGET_PRIV()

endmacro(FAST_SHARED_LIBRARY)

macro(FAST_STATIC_LIBRARY PLUGIN_NAME)

    SET(TARGET_NAME ${PLUGIN_NAME} )

    #MESSAGE("in -->SETUP_PLUGIN<-- ${TARGET_NAME}-->${TARGET_SRC} <--> ${TARGET_H}<--")

    SOURCE_GROUP( "Header Files"   FILES ${TARGET_H} ${TARGET_MOC_H} )
    SOURCE_GROUP( "Shader Files"   FILES ${TARGET_GLSL} )
    SOURCe_GROUP( "Template Files" FILES ${TARGET_IN} )

    ## we have set up the target label and targetname by taking into account global prfix (osgdb_)

    IF(NOT TARGET_TARGETNAME)
            SET(TARGET_TARGETNAME "${TARGET_DEFAULT_PREFIX}${TARGET_NAME}")
    ENDIF(NOT TARGET_TARGETNAME)
    IF(NOT TARGET_LABEL)
            SET(TARGET_LABEL "${TARGET_DEFAULT_LABEL_PREFIX} ${TARGET_NAME}")
    ENDIF(NOT TARGET_LABEL)

# here we use the command to generate the library
    ADD_LIBRARY(${TARGET_TARGETNAME} STATIC ${TARGET_SRC} ${TARGET_SRC_NO_PCH} ${TARGET_QRC} ${TARGET_H} ${TARGET_MOC_H} ${TARGET_GLSL} ${TARGET_IN})

    SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "${TARGET_LABEL}")
    SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

    SETUP_LINK_LIBRARIES()
    SETUP_OUTPUT_DIRS()

#the installation path are differentiated for win32 that install in bib versus other architecture that install in lib${LIB_POSTFIX}/${VPB_PLUGINS}
    IF(WIN32)
        INSTALL(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION bin )

    ELSE(WIN32)
        INSTALL(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin ARCHIVE DESTINATION lib${LIB_POSTFIX} LIBRARY DESTINATION lib${LIB_POSTFIX} )
    ENDIF(WIN32)

#finally, set up the solution folder -gw
    SET_PROPERTY(TARGET ${TARGET_TARGETNAME} PROPERTY FOLDER "Plugins")

    FAST_RESET_TARGET_PRIV()

endmacro(FAST_STATIC_LIBRARY)

macro(FAST_PLUGIN PLUGIN_NAME)

    cmake_parse_arguments(FAST_PLUGIN_LOCAL
		"VERBOSE"
		"PLUGINTYPE"
		""
		${ARGN})

	SET(TARGET_NO_POSTFIX 1)
	SET(TARGET_NO_LIBPREFIX 1)
	SET(TARGET_NO_IMPORT_LIB 1)
	SET(TARGET_BIN_DIR ${FAST_WORK_PLUGINS_DIR}/${FAST_PLUGIN_LOCAL_PLUGINTYPE})
	SET(TARGET_LIB_DIR ${FAST_WORK_PLUGINS_DIR}/${FAST_PLUGIN_LOCAL_PLUGINTYPE})
	SET(TARGET_INSTALL_RUNTIME_DIR "lib/plugins/${FAST_PLUGIN_LOCAL_PLUGINTYPE}")
    FAST_MODULE_LIBRARY(${PLUGIN_NAME} ${FAST_PLUGIN_LOCAL_DEFAULT_ARGS})

endmacro(FAST_PLUGIN)



#################################################################################################################
# this is the macro for example and application setup
###########################################################

MACRO(SETUP_EXE IS_COMMANDLINE_APP)
    #MESSAGE("in -->SETUP_EXE<-- ${TARGET_NAME}-->${TARGET_SRC} <--> ${TARGET_H}<--")
    IF(NOT TARGET_TARGETNAME)
            SET(TARGET_TARGETNAME "${TARGET_DEFAULT_PREFIX}${TARGET_NAME}")
    ENDIF(NOT TARGET_TARGETNAME)
    IF(NOT TARGET_LABEL)
            SET(TARGET_LABEL "${TARGET_DEFAULT_LABEL_PREFIX} ${TARGET_NAME}")
    ENDIF(NOT TARGET_LABEL)

    IF(${IS_COMMANDLINE_APP})

        ADD_EXECUTABLE(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_SRC_NO_PCH} ${TARGET_QRC} ${TARGET_H} ${TARGET_MOC_H})

    ELSE(${IS_COMMANDLINE_APP})

        IF(APPLE)
            # SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${VIRTUALPLANETBUILDER_MAJOR_VERSION}.${VIRTUALPLANETBUILDER_MINOR_VERSION}.${VIRTUALPLANETBUILDER_PATCH_VERSION}")
            # Short Version is the "marketing version". It is the version
            # the user sees in an information panel.
            SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${OSGEARTH_MAJOR_VERSION}.${OSGEARTH_MINOR_VERSION}.${OSGEARTH_PATCH_VERSION}")
            # Bundle version is the version the OS looks at.
            SET(MACOSX_BUNDLE_BUNDLE_VERSION "${OSGEARTH_MAJOR_VERSION}.${OSGEARTH_MINOR_VERSION}.${OSGEARTH__PATCH_VERSION}")
            SET(MACOSX_BUNDLE_GUI_IDENTIFIER "net.fastprotect.${TARGET_TARGETNAME}" )
            SET(MACOSX_BUNDLE_BUNDLE_NAME "${TARGET_NAME}" )
            # SET(MACOSX_BUNDLE_ICON_FILE "myicon.icns")
            # SET(MACOSX_BUNDLE_COPYRIGHT "")
            # SET(MACOSX_BUNDLE_INFO_STRING "Info string, localized?")
        ENDIF(APPLE)

        IF(WIN32)
            IF (REQUIRE_WINMAIN_FLAG)
                SET(PLATFORM_SPECIFIC_CONTROL WIN32)
            ENDIF(REQUIRE_WINMAIN_FLAG)
        ENDIF(WIN32)

        IF(APPLE)
            IF(VPB_BUILD_APPLICATION_BUNDLES)
                SET(PLATFORM_SPECIFIC_CONTROL MACOSX_BUNDLE)
            ENDIF(VPB_BUILD_APPLICATION_BUNDLES)
        ENDIF(APPLE)

        ADD_EXECUTABLE(${TARGET_TARGETNAME} ${PLATFORM_SPECIFIC_CONTROL} ${TARGET_SRC} ${TARGET_SRC_NO_PCH} ${TARGET_QRC} ${TARGET_H} ${TARGET_MOC_H})

    ENDIF(${IS_COMMANDLINE_APP})

    SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "${TARGET_LABEL}")
    SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES OUTPUT_NAME ${TARGET_NAME})
    SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES DEBUG_OUTPUT_NAME "${TARGET_NAME}${CMAKE_DEBUG_POSTFIX}")
    SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES RELEASE_OUTPUT_NAME "${TARGET_NAME}${CMAKE_RELEASE_POSTFIX}")
    SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES RELWITHDEBINFO_OUTPUT_NAME "${TARGET_NAME}${CMAKE_RELWITHDEBINFO_POSTFIX}")
    SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES MINSIZEREL_OUTPUT_NAME "${TARGET_NAME}${CMAKE_MINSIZEREL_POSTFIX}")
    SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    if(UNIX)
        set_property(TARGET ${TARGET_TARGETNAME} APPEND PROPERTY COMPILE_FLAGS "-fPIC")
    endif()

    SETUP_LINK_LIBRARIES()
    SETUP_OUTPUT_DIRS()

ENDMACRO(SETUP_EXE)

# Taked optional second arg: APPLICATION_FOLDER
# Takes optional third arg:  (is_commandline_app?) in ARGV2
MACRO(SETUP_APPLICATION APPLICATION_NAME)

    SET(TARGET_NAME ${APPLICATION_NAME} )

    # 2nd arguemnt: application folder name for IDE?
    IF(${ARGC} GREATER 1)
        SET(APPLICATION_FOLDER ${ARGV1})
    ELSE(${ARGC} GREATER 1)
        SET(APPLICATION_FOLDER ${TARGET_DEFAULT_APPLICATION_FOLDER})
    ENDIF(${ARGC} GREATER 1)

    # 3rd argument: is it a command-line app?
    IF(${ARGC} GREATER 2)
        SET(IS_COMMANDLINE_APP ${ARGV2})
    ELSE(${ARGC} GREATER 2)
        SET(IS_COMMANDLINE_APP 0)
    ENDIF(${ARGC} GREATER 2)

    SETUP_EXE(${IS_COMMANDLINE_APP})

    INSTALL(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin  )
	#Install to the OSG_DIR as well
	IF(OSGEARTH_INSTALL_TO_OSG_DIR AND OSG_DIR)
	  INSTALL(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION ${OSG_DIR}/bin)
	ENDIF(OSGEARTH_INSTALL_TO_OSG_DIR AND OSG_DIR)

	IF(NOT APPLICATION_FOLDER)
	    SET(APPLICATION_FOLDER "Examples")
	ENDIF(NOT APPLICATION_FOLDER)

	SET_PROPERTY(TARGET ${TARGET_TARGETNAME} PROPERTY FOLDER ${APPLICATION_FOLDER})

    FAST_RESET_TARGET_PRIV()

ENDMACRO(SETUP_APPLICATION)

MACRO(FAST_COMMANDLINE_APPLICATION APPLICATION_NAME)

    SETUP_APPLICATION(${APPLICATION_NAME} 1)

ENDMACRO(FAST_COMMANDLINE_APPLICATION)
