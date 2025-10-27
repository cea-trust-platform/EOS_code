# ==========================================
# CMakeLists.txt — API Python EOS (SWIG)
# ==========================================

# --- Dépendances SWIG / Python ---
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# --- Répertoires d'inclusion ---
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/Modules/
  ${CMAKE_BINARY_DIR}/Modules/
  ${CMAKE_BINARY_DIR}/Modules/EOS/API  # pour les headers générés
  ${Python3_INCLUDE_DIRS}
)

# --- Configuration SWIG ---
set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_SWIG_FLAGS "")
set_source_files_properties(EOS_py.i PROPERTIES CPLUSPLUS ON)

# --- Module SWIG ---
swig_add_library(EOS_py
  TYPE MODULE
  LANGUAGE python
  SOURCES EOS_py.i EOS_py.cxx
)

# --- Dépendance : s’assurer que la lib EOS est construite avant ---
add_dependencies(${SWIG_MODULE_EOS_py_REAL_NAME} ${NLIB_TM})

# --- Bibliothèques importées ---
add_library(LanguageSrc SHARED IMPORTED)
set_target_properties(LanguageSrc PROPERTIES
  IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/Modules/Language/Src/libLanguage_src_shar.so
)

add_library(LanguageAPI SHARED IMPORTED)
set_target_properties(LanguageAPI PROPERTIES
  IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/Modules/Language/API/libCCLanguageAPI.so
)



# --- RPATH ---
set_target_properties(${SWIG_MODULE_EOS_py_REAL_NAME} PROPERTIES
  BUILD_WITH_INSTALL_RPATH TRUE
  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
  OUTPUT_NAME "_eos_py"
)

# --- Liens ---
target_link_libraries(${SWIG_MODULE_EOS_py_REAL_NAME}
  PRIVATE
    ${NLIB_TM}
    ${Python3_LIBRARIES}
    LanguageSrc
    LanguageAPI
)

# --- Installation des bibliothèques ---
install(FILES
  ${CMAKE_BINARY_DIR}/Modules/EOS/API/libCCEOSAPI.so
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

# --- Installation du wrapper Python ---
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/eos_py.py
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

# --- Installation des libs Language nécessaires ---
install(FILES
  ${CMAKE_BINARY_DIR}/Modules/Language/Src/libLanguage_src_shar.so
  ${CMAKE_BINARY_DIR}/Modules/Language/API/libCCLanguageAPI.so
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

# --- Installation du module SWIG ---
install(TARGETS ${SWIG_MODULE_EOS_py_REAL_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

# --- Message de fin ---
message(STATUS ">>> EOS Python API (SWIG) configurée depuis ${CMAKE_CURRENT_SOURCE_DIR}")
