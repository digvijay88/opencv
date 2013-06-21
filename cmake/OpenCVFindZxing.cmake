SET(ZXING_LIBRARIES "")
SET(ZXING_LIBSEARCH_SUFFIXES "")
file(TO_CMAKE_PATH "$ENV{ProgramFiles}" ProgramFiles_ENV_PATH)

SET(LIBRARY_PATHS
    /usr/lib
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    "${ProgramFiles_ENV_PATH}/zxing/lib/static"
    "${ZXING_ROOT}/lib")

FIND_PATH(ZXING_INCLUDE_PATH
    PATH_SUFFIXES Zxing
    PATHS
    /usr/include
    /usr/local/include
    /sw/include
    /opt/local/include
    "${ProgramFiles_ENV_PATH}/zxing/include"
    "${ZXING_ROOT}/include")

IF(ZXING_FOUND)
  MESSAGE(STATUS "Found Zxing: ${ZXING_ROOT_LIBRARY}")
ELSE()
  MESSAGE(FATAL_ERROR "Could not find Zxing")
ENDIF()

MARK_AS_ADVANCED(
    ZXING_INCLUDE_PATHS
    ZXING_LIBRARIES
