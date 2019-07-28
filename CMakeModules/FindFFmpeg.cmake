
FIND_PATH(FFMPEG_INCLUDE_DIR NAMES libavcodec/avcodec.h)
FIND_LIBRARY(FFMPEG_LIBAVFORMAT_LIBRARY NAMES avformat-58 avformat-57 avformat)
FIND_LIBRARY(FFMPEG_LIBAVDEVICE_LIBRARY NAMES avdevice-58 avdevice-57 avdevice)
FIND_LIBRARY(FFMPEG_LIBAVCODEC_LIBRARY NAMES avcodec-58 avcodec-57 avcodec)
FIND_LIBRARY(FFMPEG_LIBAVUTIL_LIBRARY NAMES avutil-56 avutil-55 avutil)
FIND_LIBRARY(FFMPEG_LIBSWSCALE_LIBRARY NAMES swscale-4 swscale)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FFmpeg
                                    REQUIRED_VARS
                                        FFMPEG_INCLUDE_DIR
                                        FFMPEG_LIBAVFORMAT_LIBRARY
                                        FFMPEG_LIBAVDEVICE_LIBRARY
                                        FFMPEG_LIBAVCODEC_LIBRARY
                                        FFMPEG_LIBAVUTIL_LIBRARY
                                        FFMPEG_LIBSWSCALE_LIBRARY
                                    )
