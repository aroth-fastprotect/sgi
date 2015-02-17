# Locate Log4cplus
# This module defines
# LOG4CPLUS_LIBRARY
# LOG4CPLUS_FOUND, if false, do not try to link to Log4cplus
# LOG4CPLUS_INCLUDE_DIR, where to find the headers
#
# $LOG4CPLUS_DIR is an environment variable that would
# correspond to the ./configure --prefix=$LOG4CPLUS_DIR
# used in building osg.
#
# Created by Andreas Roth.

FIND_PATH(LOG4CPLUS_INCLUDE_DIR log4cplus/logger.h
    $ENV{LOG4CPLUS_INCLUDE_DIR}
    $ENV{LOG4CPLUS_DIR}/include
    $ENV{LOG4CPLUS_DIR}
    NO_DEFAULT_PATH
)

IF(NOT LOG4CPLUS_INCLUDE_DIR)
    FIND_PATH(LOG4CPLUS_INCLUDE_DIR log4cplus/logger.h
        PATHS ${CMAKE_PREFIX_PATH} # Unofficial: We are proposing this.
        PATH_SUFFIXES include
    )
ENDIF(NOT LOG4CPLUS_INCLUDE_DIR)

IF(NOT LOG4CPLUS_INCLUDE_DIR)
    FIND_PATH(LOG4CPLUS_INCLUDE_DIR log4cplus/logger.h
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local/include
        /usr/include
        /sw/include # Fink
        /opt/local/include # DarwinPorts
        /opt/csw/include # Blastwave
        /opt/include
    )
ENDIF(NOT LOG4CPLUS_INCLUDE_DIR)


FIND_LIBRARY(LOG4CPLUS_LIBRARY
    NAMES log4cplus
    PATHS
    $ENV{LOG4CPLUS_LIBRARY_DIR}
    $ENV{LOG4CPLUS_DIR}/lib64
    $ENV{LOG4CPLUS_DIR}/lib
    $ENV{LOG4CPLUS_DIR}
    NO_DEFAULT_PATH
)

IF(NOT LOG4CPLUS_LIBRARY)
    FIND_LIBRARY(LOG4CPLUS_LIBRARY
        NAMES log4cplus
        PATHS ${CMAKE_PREFIX_PATH} # Unofficial: We are proposing this.
        PATH_SUFFIXES lib64 lib
    )
ENDIF(NOT LOG4CPLUS_LIBRARY)

IF(NOT LOG4CPLUS_LIBRARY)
    FIND_LIBRARY(LOG4CPLUS_LIBRARY
        NAMES log4cplus
        PATHS
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local/lib64
        /usr/local/lib
        /usr/lib64
        /usr/lib
        /sw/lib64
        /sw/lib
        /opt/local/lib64
        /opt/local/lib
        /opt/csw/lib64
        /opt/csw/lib
        /opt/lib64
        /opt/lib
    )
ENDIF(NOT LOG4CPLUS_LIBRARY)


FIND_LIBRARY(LOG4CPLUS_LIBRARY_DEBUG
    NAMES log4cplusd
    PATHS
    $ENV{LOG4CPLUS_DEBUG_LIBRARY_DIR}
    $ENV{LOG4CPLUS_LIBRARY_DIR}
    $ENV{LOG4CPLUS_DIR}/lib64
    $ENV{LOG4CPLUS_DIR}/lib
    $ENV{LOG4CPLUS_DIR}
    NO_DEFAULT_PATH
)

IF(NOT LOG4CPLUS_LIBRARY_DEBUG)
    FIND_LIBRARY(LOG4CPLUS_LIBRARY_DEBUG
        NAMES log4cplusd
        PATHS ${CMAKE_PREFIX_PATH} # Unofficial: We are proposing this.
        PATH_SUFFIXES lib64 lib
    )
ENDIF(NOT LOG4CPLUS_LIBRARY_DEBUG)

IF(NOT LOG4CPLUS_LIBRARY_DEBUG)
    FIND_LIBRARY(LOG4CPLUS_LIBRARY_DEBUG
        NAMES log4cplusd
        PATHS
        /usr/local/lib64
        /usr/local/lib
        /usr/lib64
        /usr/lib
        /sw/lib64
        /sw/lib
        /opt/local/lib64
        /opt/local/lib
        /opt/csw/lib64
        /opt/csw/lib
        /opt/lib64
        /opt/lib
    )
ENDIF(NOT LOG4CPLUS_LIBRARY_DEBUG)


IF(LOG4CPLUS_LIBRARY)
  IF(NOT LOG4CPLUS_LIBRARY_DEBUG)
      #MESSAGE("-- Warning Debug Log4cplus not found, using: ${LOG4CPLUS_LIBRARY}")
      #SET(LOG4CPLUS_LIBRARY_DEBUG "${LOG4CPLUS_LIBRARY}")
      SET(LOG4CPLUS_LIBRARY_DEBUG "${LOG4CPLUS_LIBRARY}" CACHE FILEPATH "Debug version of log4cplus Library (use regular version if not available)" FORCE)
  ENDIF(NOT LOG4CPLUS_LIBRARY_DEBUG)
ENDIF(LOG4CPLUS_LIBRARY)
    
SET(LOG4CPLUS_FOUND "NO")
IF(LOG4CPLUS_INCLUDE_DIR AND LOG4CPLUS_LIBRARY)
  SET(LOG4CPLUS_FOUND "YES")
  # MESSAGE("-- Found Log4cplus: "${LOG4CPLUS_LIBRARY})
ENDIF(LOG4CPLUS_INCLUDE_DIR AND LOG4CPLUS_LIBRARY)

