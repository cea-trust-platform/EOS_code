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
# 5. Détermination de la racine des sources CoolProp
#    On remonte d'un niveau depuis static_library/ pour trouver soit :
#      - externals/  (ancienne version, sources in-tree)
#      - build/_deps (nouvelle version, dépendances fetchées par CMake)
# -----------------------------------------------------------------------
get_filename_component(_COOLPROP_BASE_DIR "${CoolProp_ROOT_HINT}" DIRECTORY)

# Chemin vers la racine des sources CoolProp (contient externals/, include/, ...)
# CoolProp_ROOT_HINT pointe vers <coolprop_src>/static_library/../  => _COOLPROP_BASE_DIR
# mais selon l'installation _COOLPROP_BASE_DIR peut être le build dir.
# On cherche externals/ d'abord à côté de static_library/, sinon on remonte.
set(_COOLPROP_EXTERNALS_DIR "")
set(_COOLPROP_DEPS_DIR "")

# Candidats pour externals/ : même niveau que static_library/, puis un niveau au-dessus
foreach(_candidate
    "${CoolProp_ROOT_HINT}/externals"
    "${_COOLPROP_BASE_DIR}/externals"
)
  if(EXISTS "${_candidate}")
    set(_COOLPROP_EXTERNALS_DIR "${_candidate}")
    message(STATUS "FindCoolProp: externals/ found : ${_COOLPROP_EXTERNALS_DIR}")
    break()
  endif()
endforeach()

# Candidats pour build/_deps (nouvelle version)
if(NOT _COOLPROP_EXTERNALS_DIR)
  set(_COOLPROP_DEPS_CANDIDATE "${_COOLPROP_BASE_DIR}/build/_deps")
  if(EXISTS "${_COOLPROP_DEPS_CANDIDATE}")
    set(_COOLPROP_DEPS_DIR "${_COOLPROP_DEPS_CANDIDATE}")
    message(STATUS "FindCoolProp: build/_deps found : ${_COOLPROP_DEPS_DIR}")
  else()
    message(WARNING "FindCoolProp: neither externals/ nor build/_deps found. "
                    "Dependency headers (fmt, eigen, ...) may be missing.")
  endif()
endif()

# -----------------------------------------------------------------------
# 6. Collecte des includes des dépendances
# -----------------------------------------------------------------------
set(_COOLPROP_DEP_INCLUDES "")

# ---- 6a. Ancienne version : externals/ --------------------------------
if(_COOLPROP_EXTERNALS_DIR)
  # Format : <nom-dossier-dans-externals>  <sous-dossier-include-relatif>
  # Laisser le sous-dossier vide "" si la racine du dépôt EST l'include path.
  set(_COOLPROP_EXT_MAP
    "fmtlib"         ""   # externals/fmtlib/fmt/...
    "Eigen"          ""          # externals/Eigen/ (headers à la racine)
    "msgpack-c"      "include"   # externals/msgpack-c/include/
    "rapidjson"      "include"   # externals/rapidjson/include/
    "IF97"           ""          # externals/IF97/
    "multicomplex"   ""          # externals/multicomplex/
    "REFPROP-headers" ""         # externals/REFPROP-headers/
  )

  list(LENGTH _COOLPROP_EXT_MAP _ext_map_len)
  set(_i 0)
  while(_i LESS _ext_map_len)
    list(GET _COOLPROP_EXT_MAP ${_i} _dep_name)
    math(EXPR _i_next "${_i} + 1")
    list(GET _COOLPROP_EXT_MAP ${_i_next} _dep_sub)
    math(EXPR _i "${_i} + 2")

    set(_dep_path "${_COOLPROP_EXTERNALS_DIR}/${_dep_name}")
    if(EXISTS "${_dep_path}")
      if(_dep_sub STREQUAL "")
        list(APPEND _COOLPROP_DEP_INCLUDES "${_dep_path}")
        message(STATUS "FindCoolProp: ext include [${_dep_name}] : ${_dep_path}")
      else()
        set(_dep_inc "${_dep_path}/${_dep_sub}")
        if(EXISTS "${_dep_inc}")
          list(APPEND _COOLPROP_DEP_INCLUDES "${_dep_inc}")
          message(STATUS "FindCoolProp: ext include [${_dep_name}] : ${_dep_inc}")
        else()
          message(WARNING "FindCoolProp: include subdir not found for [${_dep_name}] : ${_dep_inc}")
        endif()
      endif()
    else()
      message(STATUS "FindCoolProp: ext dep [${_dep_name}] not present, skipping")
    endif()
  endwhile()

# ---- 6b. Nouvelle version : build/_deps -------------------------------
elseif(_COOLPROP_DEPS_DIR)
  set(_COOLPROP_DEP_MAP
    "fmt-src"              "include"
    "eigen-src"            ""
    "msgpack-c-src"        "include"
    "rapidjson-src"        "include"
    "if97-src"             ""
    "multicomplex-src"     ""
    "boost_headers-src"    ""
    "refprop_headers-src"  ""
  )

  list(LENGTH _COOLPROP_DEP_MAP _dep_map_len)
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
find_package_handle_standard_args(CoolProp
  REQUIRED_VARS
    COOLPROP_STATICLIB
    COOLPROP_MAIN_HEADER
  VERSION_VAR
    CoolProp_VERSION
)

# Variables en cache pour l'inspecteur CMake
set(COOLPROP_INCLUDE_DIRS "${COOLPROP_INCLUDE_DIRS}" CACHE STRING "CoolProp include directories" FORCE)
set(COOLPROP_STATICLIB    "${COOLPROP_STATICLIB}"    CACHE FILEPATH "CoolProp static library"    FORCE)
set(COOLPROP_SHAREDLIB    "${COOLPROP_SHAREDLIB}"    CACHE FILEPATH "CoolProp shared library" FORCE)

mark_as_advanced(COOLPROP_INCLUDE_DIRS COOLPROP_STATICLIB COOLPROP_SHAREDLIB COOLPROP_MAIN_HEADER)