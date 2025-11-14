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