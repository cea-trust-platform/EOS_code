if (NOT WITH_DOC_SPHINX)
    return()
endif()

message(STATUS "Configuring Sphinx documentation (Doc Sphinx)")

# ------------------------------------------------------------------------------
# Python / Sphinx dependencies
# ------------------------------------------------------------------------------

find_package(Python3 COMPONENTS Interpreter REQUIRED)


execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import breathe"
    RESULT_VARIABLE BREATHE_RESULT
    OUTPUT_QUIET
    ERROR_QUIET
)

if (NOT BREATHE_RESULT EQUAL 0)
    message(FATAL_ERROR
        "\n"
        "Breathe is required to build the Sphinx documentation but was not found.\n"
        "\n"
        "Please create and activate a Python virtual environment:\n"
        "\n"
        "  python3 -m venv <venv>\n"
        "  source <venv>/bin/activate\n"
        "  python -m pip install -r ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt\n"
        "\n"
        "Then re-run CMake.\n"
    )
endif()
# ------------------------------------------------------------------------------
# Directories
# ------------------------------------------------------------------------------

set(SPHINX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD_DIR  ${CMAKE_CURRENT_BINARY_DIR}/_build)

# ------------------------------------------------------------------------------
# Build Sphinx
# ------------------------------------------------------------------------------

add_custom_target(doc_sphinx
    COMMAND sphinx-build
        -b html
        ${SPHINX_SOURCE_DIR}/source
        ${SPHINX_BUILD_DIR}/html
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building Sphinx HTML documentation (Doc_EOS)"
)

# ------------------------------------------------------------------------------
# Force build before install
# ------------------------------------------------------------------------------

install(
    CODE "
    execute_process(
        COMMAND \"${CMAKE_COMMAND}\" --build . --target doc_sphinx
        WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}\"
    )
    "
)

# ------------------------------------------------------------------------------
# Install FULL _build directory
# ------------------------------------------------------------------------------

install(
    DIRECTORY ${SPHINX_BUILD_DIR}/
    DESTINATION sphinx/_build
    COMPONENT doc
)

message(STATUS "Sphinx documentation will be built and installed")