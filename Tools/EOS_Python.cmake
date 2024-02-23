#
#
#          Handling of Python pre-requisite for EOS GUI
#
#
#       There are 3 ways of finding Python:
#
#           1) let cmake find it
#
#           2) specify -DUSER_PYTHON_EXECUTABLE=/path/to/python3
#              If it is executable, don't do anything else.
#              If it is not, either append /path/to/python/lib to your LD_LIBRARY_PATH,
#              or specify also -DUSER_PYTHON_LIB=/path/to/python/lib
#
#           3) specify the 3 following variables:
#               -DUSER_PYTHON_EXECUTABLE=/path/to/python3                  |path to executable
#               -DUSER_PYTHON_LIB=/path/to/python/lib/libpythonX.Y.[so|a]  |full path to library file
#               -DUSER_PYTHON_INC=/path/to/python/include/pythonX.Y        |path to directory containing Python.h
#
#
#
#       Return Values:
#       ^^^^^^^^^^^^^
#           Python_EXECUTABLE
#           Python_LIBRARY_DIRS
#           Python_LIBRARIES
#           Python_INCLUDES
#           EOS_PYTHON_SITE_PACKAGES_DIR  |path to install directory of EOS python module




if(NOT USER_PYTHON_INC)
  set(ENV{LD_LIBRARY_PATH} $ENV{LD_LIBRARY_PATH}:${USER_PYTHON_LIB})
  set(Python_EXECUTABLE ${USER_PYTHON_EXECUTABLE})
  find_package(Python MODULE COMPONENTS Interpreter Development REQUIRED)

  # Sanity checks
  if(USER_PYTHON_EXECUTABLE)
    get_filename_component(USER_PY_EXECUTABLE ${USER_PYTHON_EXECUTABLE} REALPATH)
    get_filename_component(FOUND_PY_EXECUTABLE ${Python_EXECUTABLE} REALPATH)
    if(NOT USER_PY_EXECUTABLE STREQUAL FOUND_PY_EXECUTABLE)
      message(SEND_ERROR "\nfind_package(Python) found an other python than the one specified by user.\n"
                      "You specified: ${USER_PY_EXECUTABLE}\n"
                      "We found:      ${FOUND_PY_EXECUTABLE}\n"
                      "Try specifying the directory containing libpython<version>.so with \"--with-python-libs=...\" configure option\n"
                      "or with \"-DUSER_PYTHON_LIB:PATH=...\" cmake option.\n"
                      "or append the path to python shared lib to your LD_LIBRARY_PATH\n"
                      "You may need to clean your build directory first."
      )
    endif()
  endif()

# Case where python-dev is custom
else(NOT USER_PYTHON_INC)
  if(NOT USER_PYTHON_EXECUTABLE OR NOT USER_PYTHON_LIB)
    message(FATAL_ERROR "If you want to specify the python include directory, you must also specify "
                        "the path to the associated python executable, and shared library file.")
  endif()
  message(WARNING "By setting the python include directory, you are specifying a custom python path. "
                  "We recommend to specify only the python executable, and add its library directory to your LD_LIBRARY_PATH, "
                  "or to set -DUSER_PYTHON_LIB=/path/to/python/lib")
  get_filename_component(Python_EXECUTABLE ${USER_PYTHON_EXECUTABLE} REALPATH CACHE)
  get_filename_component(Python_INCLUDE_DIRS ${USER_PYTHON_INC} REALPATH CACHE)
  get_filename_component(Python_LIBRARIES ${USER_PYTHON_LIB} REALPATH CACHE)
  get_filename_component(Python_LIBRARY_DIRS ${USER_PYTHON_LIB} DIRECTORY CACHE)
  if(NOT EXISTS ${Python_EXECUTABLE})
    message(SEND_ERROR "Python executable provided by user not found:\n${USER_PYTHON_EXECUTABLE}")
  endif()
  if(NOT EXISTS ${Python_LIBRARIES})
    message(SEND_ERROR "Python library provided by user not found:\n${USER_PYTHON_LIB}\n"
                       "Note that it must point to the library file .so or .a as you specified python include directory.")
  endif()
  if(NOT EXISTS "${Python_INCLUDE_DIRS}/Python.h")
    message(SEND_ERROR "Python header file Python.h not found in directory specified by user:\n${USER_PYTHON_INC}")
  endif()
endif(NOT USER_PYTHON_INC)

message(STATUS "Python executable: ${Python_EXECUTABLE}")
message(STATUS "Python libraries:  ${Python_LIBRARIES}")
message(STATUS "Python includes:   ${Python_INCLUDE_DIRS}")
set(EOS_PYTHON_SITE_PACKAGES_DIR ${EOS_LIB_DIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages/eos CACHE PATH "new site-package path" FORCE)
mark_as_advanced(FORCE EOS_PYTHON_SITE_PACKAGES_DIR)
