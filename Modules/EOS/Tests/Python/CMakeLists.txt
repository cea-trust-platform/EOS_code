# ==========================================
# Test Python pour l’API EOS (SWIG)
# ==========================================
message(STATUS "Add test PythonApi")
# (add python test EOSTest_PyAPI)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(PY_TEST_SCRIPT ${CMAKE_SOURCE_DIR}/Modules/EOS/Tests/Python/test_eos_py.py)

if(EXISTS ${PY_TEST_SCRIPT})
  message(STATUS "Add test EOSTest_PyAPI (Python version)")
  
  # Ajout du test CTest
  add_test(NAME EOSTest_PyAPI
           COMMAND ${Python3_EXECUTABLE} ${PY_TEST_SCRIPT}
           WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib)

  # On peut définir les variables d’environnement si besoin
  set_tests_properties(EOSTest_PyAPI PROPERTIES
    ENVIRONMENT
      "PYTHONPATH=${CMAKE_INSTALL_PREFIX}/lib"
    COST 2.
  )
else()
  message(WARNING "EOSTest_PyAPI not added: test file not found at ${PY_TEST_SCRIPT}")
endif()

# ==========================================
# Test Python de l’interpolateur EOS_Ipp (bilineaire / bicubique)
# ==========================================
if(WITH_IPP)
  set(PY_TEST_SCRIPT_IPP_BICUBIC ${CMAKE_SOURCE_DIR}/Modules/EOS/Tests/Python/test_eos_ipp_bicubic.py)

  if(EXISTS ${PY_TEST_SCRIPT_IPP_BICUBIC})
    message(STATUS "Add test EOSTest_PyAPI_IppBicubic (Python version)")

    add_test(NAME EOSTest_PyAPI_IppBicubic
             COMMAND ${Python3_EXECUTABLE} ${PY_TEST_SCRIPT_IPP_BICUBIC}
             WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib)

    # EOS_IGen_py::write_med ecrit sous $USER_EOS_DATA/EOS_Ipp/ ; DEPENDS
    # EOSIGenTest serialise l'acces au index.eos partage avec les autres
    # tests qui y ecrivent (meme pattern que EOSTestIpp/EOSTestIppBicubic,
    # cf. Modules/EOS/Tests/C++/CMakeLists.txt).
    set_tests_properties(EOSTest_PyAPI_IppBicubic PROPERTIES
      ENVIRONMENT
        "PYTHONPATH=${CMAKE_INSTALL_PREFIX}/lib;USER_EOS_DATA=${CMAKE_BINARY_DIR}/Modules/EOS/Data/"
      DEPENDS EOSIGenTest
      COST 5.
    )
  else()
    message(WARNING "EOSTest_PyAPI_IppBicubic not added: test file not found at ${PY_TEST_SCRIPT_IPP_BICUBIC}")
  endif()
endif(WITH_IPP)

# ==========================================
# Test Python du melange EOS_Mixing
# ==========================================
set(PY_TEST_SCRIPT_MIXING ${CMAKE_SOURCE_DIR}/Modules/EOS/Tests/Python/test_eos_mixing.py)

if(EXISTS ${PY_TEST_SCRIPT_MIXING})
  message(STATUS "Add test EOSTest_PyAPI_Mixing (Python version)")

  add_test(NAME EOSTest_PyAPI_Mixing
           COMMAND ${Python3_EXECUTABLE} ${PY_TEST_SCRIPT_MIXING}
           WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib)

  set_tests_properties(EOSTest_PyAPI_Mixing PROPERTIES
    ENVIRONMENT
      "PYTHONPATH=${CMAKE_INSTALL_PREFIX}/lib"
    COST 2.
  )
else()
  message(WARNING "EOSTest_PyAPI_Mixing not added: test file not found at ${PY_TEST_SCRIPT_MIXING}")
endif()