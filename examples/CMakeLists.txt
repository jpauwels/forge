CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(Forge-Examples)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")

MACRO(BUILD_EXAMPLE EX_NAME EX_SRC COMPUTE_NAME FG_LIBS COMPUTE_LIBS)
    IF(${COMPUTE_NAME} STREQUAL "cuda")
        CUDA_ADD_EXECUTABLE(example_${EX_NAME}_${COMPUTE_NAME} ${EX_SRC})
    ELSE()
        ADD_EXECUTABLE(example_${EX_NAME}_${COMPUTE_NAME} ${EX_SRC})
    ENDIF()
    TARGET_LINK_LIBRARIES(example_${EX_NAME}_${COMPUTE_NAME} ${COMPUTE_LIBS} ${FG_LIBS})
    SET_TARGET_PROPERTIES(example_${EX_NAME}_${COMPUTE_NAME}
        PROPERTIES
        OUTPUT_NAME ${EX_NAME}_${COMPUTE_NAME}
        RUNTIME_OUTPUT_DIRECTORY ${DIR_NAME}
        FOLDER "Examples/${COMPUTE_NAME}")
ENDMACRO()

IF(TARGET forge)
    # Building with Forge Source
    SET(FORGE_FOUND False)
ELSE(TARGET forge)
    SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
    FIND_PACKAGE(GLEWmx REQUIRED)

    FIND_PACKAGE(OpenGL REQUIRED)
    FIND_PACKAGE(Forge REQUIRED)
    SET(X11_LIBS "")
    IF(APPLE)
        FIND_PACKAGE(X11 REQUIRED)
        INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
    ENDIF(APPLE)
    INCLUDE_DIRECTORIES(
        ${FORGE_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIR}
        )
    ADD_DEFINITIONS(-DGLEW_MX)
IF(UNIX)
    ADD_DEFINITIONS(-Wall -std=c++11 -fvisibility=hidden)
ENDIF()

# OS Definitions
IF(UNIX)
    IF(APPLE)
        ADD_DEFINITIONS(-DOS_MAC)
        SET(CMAKE_MACOSX_RPATH ON)
        SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
        SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
        SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}")
        SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

        LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}" isSystemDir)
        IF("${isSystemDir}" STREQUAL "-1")
            SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}")
        ENDIF("${isSystemDir}" STREQUAL "-1")
    ELSE(APPLE)
        ADD_DEFINITIONS(-DOS_LNX)
    ENDIF(APPLE)
ELSE(UNIX)
    ADD_DEFINITIONS(-DOS_WIN -DNOMINMAX)
ENDIF(UNIX)
ENDIF(TARGET forge)

FILE(GLOB CPU_EXAMPLE_SRC_FILES "cpu/*.cpp")
FOREACH(FILE ${CPU_EXAMPLE_SRC_FILES})
    GET_FILENAME_COMPONENT(EXAMPLE ${FILE} NAME_WE)
    GET_FILENAME_COMPONENT(FULL_DIR_NAME ${FILE} PATH)
    GET_FILENAME_COMPONENT(DIR_NAME ${FULL_DIR_NAME} NAME)

    IF(TARGET forge)
        BUILD_EXAMPLE(${EXAMPLE} ${FILE} ${DIR_NAME} forge
                      "${GLEWmx_LIBRARY};${OPENGL_gl_LIBRARY}")
    ELSE(TARGET forge)
        BUILD_EXAMPLE(${EXAMPLE} ${FILE} ${DIR_NAME} ${FORGE_LIBRARIES}
            "${GLEWmx_LIBRARY};${OPENGL_gl_LIBRARY};${X11_LIBS};")
    ENDIF()
ENDFOREACH()

FIND_PACKAGE(CUDA QUIET)
IF(CUDA_FOUND)
    INCLUDE_DIRECTORIES(${CUDA_INCLUDE_DIRS})

    FILE(GLOB CUDA_EXAMPLE_SRC_FILES "cuda/*.cu")
    FOREACH(FILE ${CUDA_EXAMPLE_SRC_FILES})
        GET_FILENAME_COMPONENT(EXAMPLE ${FILE} NAME_WE)
        GET_FILENAME_COMPONENT(FULL_DIR_NAME ${FILE} PATH)
        GET_FILENAME_COMPONENT(DIR_NAME ${FULL_DIR_NAME} NAME)

        IF(TARGET forge)
              BUILD_EXAMPLE(${EXAMPLE} ${FILE} ${DIR_NAME} forge
                            "${CUDA_CUDA_LIBRARY};${CUDA_LIBRARIES}")
        ELSE(TARGET forge)
              BUILD_EXAMPLE(${EXAMPLE} ${FILE} ${DIR_NAME} ${FORGE_LIBRARIES}
                            "${GLEWmx_LIBRARY};${OPENGL_gl_LIBRARY};${X11_LIBS};${CUDA_CUDA_LIBRARY};${CUDA_LIBRARIES}")
        ENDIF()
    ENDFOREACH()
ELSE()
    MESSAGE(STATUS "CUDA Toolkit not found, not building CUDA examples.")
ENDIF()

FIND_PACKAGE(OpenCL QUIET)
IF(OpenCL_FOUND)
    IF(UNIX)
      SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations -Wno-unused-function")
    ENDIF()
    FILE(GLOB OpenCL_EXAMPLE_SRC_FILES "opencl/*.cpp")
    INCLUDE_DIRECTORIES(
        "${CMAKE_CURRENT_SOURCE_DIR}/opencl"
        ${OpenCL_INCLUDE_DIRS}
        )
    FOREACH(FILE ${OpenCL_EXAMPLE_SRC_FILES})
        GET_FILENAME_COMPONENT(EXAMPLE ${FILE} NAME_WE)
        GET_FILENAME_COMPONENT(FULL_DIR_NAME ${FILE} PATH)
        GET_FILENAME_COMPONENT(DIR_NAME ${FULL_DIR_NAME} NAME)

        IF(TARGET forge)
              BUILD_EXAMPLE(${EXAMPLE} ${FILE} ${DIR_NAME} forge
                            "${GLEWmx_LIBRARY};${OPENGL_gl_LIBRARY};${OpenCL_LIBRARIES}")
        ELSE(TARGET forge)
              BUILD_EXAMPLE(${EXAMPLE} ${FILE} ${DIR_NAME} ${FORGE_LIBRARIES}
                            "${GLEWmx_LIBRARY};${OPENGL_gl_LIBRARY};${X11_LIBS};${OpenCL_LIBRARIES}")
        ENDIF()
    ENDFOREACH()
ELSE()
    MESSAGE(STATUS "OpenCL Libraries not found, not building OpenCL examples.")
ENDIF()
