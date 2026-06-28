cmake_minimum_required(VERSION 3.22)

project(
    ParquetWriter
    VERSION 1.0.0
    LANGUAGES CXX
)

find_package(COLA REQUIRED)
find_package(Arrow REQUIRED)
find_package(Parquet REQUIRED)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_INSTALL_PREFIX ${COLA_DIR})
set(COLA_MODULE_NAME ParquetWriter)

set(COLA_CMAKE_CXX_CLANG_TIDY
    ""
    CACHE STRING "Passed to CMAKE_CXX_CLANG_TIDY for ${COLA_MODULE_NAME} targets"
)

set(SRCS
    ParquetWriter/ParquetWriter.cc
    ParquetWriter/ParquetWriterModule.cc
)

add_library(${COLA_MODULE_NAME} SHARED ${SRCS})

if (COLA_CMAKE_CXX_CLANG_TIDY)
    set_source_files_properties(
        ${SRCS} PROPERTIES CXX_CLANG_TIDY "${COLA_CMAKE_CXX_CLANG_TIDY}"
    )
endif ()

target_compile_features(${COLA_MODULE_NAME} PRIVATE cxx_std_17)

target_include_directories(
    ${COLA_MODULE_NAME}
    PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/ParquetWriter>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${COLA_MODULE_NAME}>
)

target_link_libraries(
    ${COLA_MODULE_NAME}
    PUBLIC COLA Arrow::arrow_shared Parquet::parquet_shared
)

set(PROJECT_PUBLIC_HEADERS
    "ParquetWriter/ParquetWriter.hh"
    "ParquetWriter/ParquetWriterFactory.hh"
)

set_target_properties(
    ${COLA_MODULE_NAME}
    PROPERTIES PUBLIC_HEADER "${PROJECT_PUBLIC_HEADERS}"
               VERSION "${PROJECT_VERSION}"
               SOVERSION "${PROJECT_VERSION_MAJOR}"
               BUILD_WITH_INSTALL_RPATH TRUE
               INSTALL_RPATH_USE_LINK_PATH TRUE
)

if (NOT DEFINED ENV{COLA_FETCH_MODE})
    configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/${COLA_MODULE_NAME}Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${COLA_MODULE_NAME}Config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${COLA_MODULE_NAME}
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${COLA_MODULE_NAME}ConfigVersion.cmake
        COMPATIBILITY AnyNewerVersion
    )

    install(
        TARGETS ${COLA_MODULE_NAME}
        EXPORT ${COLA_MODULE_NAME}Export
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${COLA_MODULE_NAME}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${COLA_MODULE_NAME}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${COLA_MODULE_NAME}
    )

    install(
        EXPORT ${COLA_MODULE_NAME}Export
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${COLA_MODULE_NAME}
    )

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${COLA_MODULE_NAME}Config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/${COLA_MODULE_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${COLA_MODULE_NAME}
    )

    enable_testing()
    if (BUILD_TESTING)
        add_subdirectory(tests)
    endif ()

    setup_cola_quality_tools()
endif ()