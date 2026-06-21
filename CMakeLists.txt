cmake_minimum_required(VERSION 3.16)
project(ParquetWriterModule VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(COLA REQUIRED)
find_package(Arrow REQUIRED)
find_package(Parquet REQUIRED)

add_library(ParquetWriter SHARED
    ParquetWriter/ParquetWriter.cc
    ParquetWriter/ParquetWriterModule.cc
)
target_include_directories(ParquetWriter PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/ParquetWriter")
target_link_libraries(ParquetWriter PUBLIC COLA Arrow::arrow_shared Parquet::parquet_shared)
set_target_properties(ParquetWriter PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
)

install(TARGETS ParquetWriter LIBRARY DESTINATION lib/ParquetWriter)

enable_testing()

add_test(
    NAME parquet_writer_tests
    COMMAND ${CMAKE_COMMAND} -E env
            "LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib"
            python -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/tests -v
)