if(USE_TRACY)
    # Compile TracyClient.cpp once as OBJECT library.
    add_library(TracyObjects OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/public/TracyClient.cpp)
    target_include_directories(TracyObjects PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)
    target_compile_definitions(TracyObjects PUBLIC TRACY_ENABLE)

    # Add Tracy objects to LCOMM_TGT_OBJ → they get merged into libeos.a/libeos.so
    # by the root CMakeLists.txt (same mechanism as all other modules).
    set(LCOMM_TGT_OBJ ${LCOMM_TGT_OBJ} TracyObjects CACHE STRING LCOMM_TGT_OBJ FORCE)

    # Static library built from the same objects (no recompilation).
    # Needed so that test executables and intermediate shared libs can link against
    # it and export Tracy symbols to resolve the .so's undefined references at runtime.
    add_library(tracy STATIC $<TARGET_OBJECTS:TracyObjects>)
    target_include_directories(tracy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)
    target_compile_definitions(tracy PUBLIC TRACY_ENABLE)

    message(STATUS "Tracy profiling : ON (merged into libeos.a)")
else()
    # INTERFACE-only stub: headers available, TRACY_ENABLE not set →
    # all Tracy macros expand to nothing, zero overhead.
    add_library(tracy INTERFACE)
    target_include_directories(tracy INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/public)

    message(STATUS "Tracy profiling : OFF")
endif()