# FindCoolProp.cmake
# ------------------
# Recherche CoolProp en mode statique uniquement.
#
# Variables d'entrée :
#   CoolProp_ROOT_HINT    : chemin vers install_root de libcoolprop (--with-libcoolprop)
#   CoolProp_PLUGIN_DIR   : chemin vers les sources du plugin EOS   (--with-coolprop)
#
# Variables de sortie :
#   CoolProp_FOUND
#   COOLPROP_INCLUDE_DIRS
#   COOLPROP_STATICLIB

# -----------------------------------------------------------------------
# 1. Vérifications préliminaires
# -----------------------------------------------------------------------
if(NOT CoolProp_ROOT_HINT)
  message(FATAL_ERROR "FindCoolProp: CoolProp_ROOT_HINT is not set. "
                      "Please provide --with-libcoolprop=<path>")
endif()

if(NOT CoolProp_PLUGIN_DIR)
  message(FATAL_ERROR "FindCoolProp: CoolProp_PLUGIN_DIR is not set. "
                      "Please provide --with-coolprop=<path>")
endif()

# -----------------------------------------------------------------------
# 2. Vérification de l'existence du dossier static_library
# -----------------------------------------------------------------------
set(_COOLPROP_STATIC_ROOT "${CoolProp_ROOT_HINT}/static_library")

if(NOT EXISTS "${_COOLPROP_STATIC_ROOT}")
  message(FATAL_ERROR "FindCoolProp: static_library directory not found in "
                      "${CoolProp_ROOT_HINT}. "
                      "Expected: ${_COOLPROP_STATIC_ROOT}")
endif()
message(STATUS "FindCoolProp: static_library directory found : ${_COOLPROP_STATIC_ROOT}")

# -----------------------------------------------------------------------
# 3. Header principal : CoolPropLib.h (dans static_library/)
# -----------------------------------------------------------------------
find_file(COOLPROP_MAIN_HEADER
  NAMES CoolPropLib.h
  PATHS "${_COOLPROP_STATIC_ROOT}"
  NO_DEFAULT_PATH
)

if(NOT COOLPROP_MAIN_HEADER)
  message(FATAL_ERROR "FindCoolProp: CoolPropLib.h not found in ${_COOLPROP_STATIC_ROOT}")
endif()
message(STATUS "FindCoolProp: CoolPropLib.h found : ${COOLPROP_MAIN_HEADER}")

# -----------------------------------------------------------------------
# 4. Recherche récursive de libCoolProp.a sous static_library/
# -----------------------------------------------------------------------
file(GLOB_RECURSE _COOLPROP_STATIC_LIB_CANDIDATES
  "${_COOLPROP_STATIC_ROOT}/libCoolProp.a"
)

if(NOT _COOLPROP_STATIC_LIB_CANDIDATES)
  message(FATAL_ERROR "FindCoolProp: libCoolProp.a not found recursively under "
                      "${_COOLPROP_STATIC_ROOT}")
endif()

# On prend le premier trouvé
list(GET _COOLPROP_STATIC_LIB_CANDIDATES 0 COOLPROP_STATICLIB)
message(STATUS "FindCoolProp: libCoolProp.a found : ${COOLPROP_STATICLIB}")


# -----------------------------------------------------------------------
# 5. Recherche du dossier build/_deps
# -----------------------------------------------------------------------
get_filename_component(_COOLPROP_BASE_DIR "${CoolProp_ROOT_HINT}" DIRECTORY)

set(_COOLPROP_DEPS_DIR "")
set(_COOLPROP_DEPS_CANDIDATE "${_COOLPROP_BASE_DIR}/build/_deps")

if(EXISTS "${_COOLPROP_DEPS_CANDIDATE}")
  set(_COOLPROP_DEPS_DIR "${_COOLPROP_DEPS_CANDIDATE}")
  message(STATUS "FindCoolProp: build/_deps found : ${_COOLPROP_DEPS_DIR}")
else()
  message(WARNING "FindCoolProp: build/_deps not found in ${_COOLPROP_BASE_DIR}/build/. "
                  "Dependency headers (fmt, eigen, ...) may be missing.")
endif()
# -----------------------------------------------------------------------
# 6. Collecte des includes des dépendances dans _deps
# -----------------------------------------------------------------------
set(_COOLPROP_DEP_INCLUDES "")

if(_COOLPROP_DEPS_DIR)
  # Liste des dépendances connues de CoolProp et leurs sous-dossiers d'include
  # Format : <nom-src>  <sous-dossier-include-relatif>
  set(_COOLPROP_DEP_MAP
    "fmt-src"              "include"
    "eigen-src"            ""           # eigen : la racine est l'include
    "msgpack-c-src"        "include"
    "rapidjson-src"        "include"
    "if97-src"             ""
    "multicomplex-src"     ""
    "boost_headers-src"    ""
    "refprop_headers-src"  ""
  )

  # On itère par paires (nom, sous-dossier)
  list(LENGTH _COOLPROP_DEP_MAP _dep_map_len)
  math(EXPR _dep_map_last "${_dep_map_len} - 1")

  set(_i 0)
  while(_i LESS _dep_map_len)
    list(GET _COOLPROP_DEP_MAP ${_i} _dep_name)
    math(EXPR _i_next "${_i} + 1")
    list(GET _COOLPROP_DEP_MAP ${_i_next} _dep_sub)
    math(EXPR _i "${_i} + 2")

    set(_dep_path "${_COOLPROP_DEPS_DIR}/${_dep_name}")
    if(EXISTS "${_dep_path}")
      if(_dep_sub STREQUAL "")
        list(APPEND _COOLPROP_DEP_INCLUDES "${_dep_path}")
        message(STATUS "FindCoolProp: dep include [${_dep_name}] : ${_dep_path}")
      else()
        set(_dep_inc "${_dep_path}/${_dep_sub}")
        if(EXISTS "${_dep_inc}")
          list(APPEND _COOLPROP_DEP_INCLUDES "${_dep_inc}")
          message(STATUS "FindCoolProp: dep include [${_dep_name}] : ${_dep_inc}")
        else()
          message(WARNING "FindCoolProp: include subdir not found for [${_dep_name}] : ${_dep_inc}")
        endif()
      endif()
    else()
      message(STATUS "FindCoolProp: dep [${_dep_name}] not present, skipping")
    endif()
  endwhile()
endif()

# -----------------------------------------------------------------------
# 7. Assemblage final de COOLPROP_INCLUDE_DIRS
# -----------------------------------------------------------------------


set(COOLPROP_INCLUDE_DIRS
  "${_COOLPROP_STATIC_ROOT}"            # CoolPropLib.h  (static_library/)
  "${_COOLPROP_BASE_DIR}/include"       # DataStructures.h et autres headers CoolProp
  ${_COOLPROP_DEP_INCLUDES}             # fmt, eigen, msgpack, ...
)

# Vérification explicite
if(NOT EXISTS "${_COOLPROP_BASE_DIR}/include/DataStructures.h")
  message(WARNING "FindCoolProp: DataStructures.h not found in "
                  "${_COOLPROP_BASE_DIR}/include. "
                  "Check your CoolProp installation.")
endif()

# -----------------------------------------------------------------------
# 7.5 Recherche de libCoolProp.so (Shared Library)
# -----------------------------------------------------------------------
set(_COOLPROP_SHARED_ROOT "${CoolProp_ROOT_HINT}/shared_library/Linux/64bit")

find_library(COOLPROP_SHAREDLIB
  NAMES CoolProp CoolPropLib
  PATHS "${_COOLPROP_SHARED_ROOT}"
  NO_DEFAULT_PATH
)

if(COOLPROP_SHAREDLIB)
  message(STATUS "FindCoolProp: libCoolProp.so found : ${COOLPROP_SHAREDLIB}")
else()
  message(WARNING "FindCoolProp: libCoolProp.so not found under ${_COOLPROP_SHARED_ROOT}")
endif()

# -----------------------------------------------------------------------
# 8. Validation finale via find_package_handle_standard_args
# -----------------------------------------------------------------------
include(FindPackageHandleStandardArgs)
# On demande au moins la lib statique OU la dynamique pour valider le package
find_package_handle_standard_args(CoolProp
  REQUIRED_VARS
    COOLPROP_MAIN_HEADER
    COOLPROP_STATICLIB
  VERSION_VAR
    CoolProp_VERSION
)

# Variables en cache pour l'inspecteur CMake
set(COOLPROP_INCLUDE_DIRS "${COOLPROP_INCLUDE_DIRS}" CACHE STRING "CoolProp include directories" FORCE)
set(COOLPROP_STATICLIB    "${COOLPROP_STATICLIB}"    CACHE FILEPATH "CoolProp static library"    FORCE)
set(COOLPROP_SHAREDLIB    "${COOLPROP_SHAREDLIB}"    CACHE FILEPATH "CoolProp shared library"    FORCE) # <-- MODIFIÉ ICI

mark_as_advanced(COOLPROP_INCLUDE_DIRS COOLPROP_STATICLIB COOLPROP_SHAREDLIB COOLPROP_MAIN_HEADER)

