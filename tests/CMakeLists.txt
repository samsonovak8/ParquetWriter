set(Python3_FIND_VIRTUALENV FIRST)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

execute_process(
    COMMAND "${Python3_EXECUTABLE}" -m pytest --version
    RESULT_VARIABLE _pytest_found
    OUTPUT_QUIET
    ERROR_QUIET
)
if (_pytest_found)
    message(
        FATAL_ERROR
        "pytest not found for ${Python3_EXECUTABLE}. "
    )
endif ()

add_test(
    NAME parquet_writer_tests
    COMMAND ${CMAKE_COMMAND} -E env
            "LD_LIBRARY_PATH=${COLA_DIR}/${CMAKE_INSTALL_LIBDIR}"
            ${Python3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR} -v
)