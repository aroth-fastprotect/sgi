FIND_PATH( LOG4CPLUS_INCLUDE_DIR
    NAMES
        log4cplus/logger.h
)

FIND_LIBRARY(LOG4CPLUS_LIBRARY
    NAMES
        log4cplus
    PATH_SUFFIXES
        /lib/
        /lib64/
        /build/lib/
        /build/lib64/
        /Build/lib/
        /Build/lib64/
     )


include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Log4cplus
                                    REQUIRED_VARS LOG4CPLUS_INCLUDE_DIR LOG4CPLUS_LIBRARY
                                    )
