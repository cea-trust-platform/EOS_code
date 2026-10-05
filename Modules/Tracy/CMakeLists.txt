if(USE_TRACY)
    # Tracy keeps process-wide state (event queues, profiler thread, static
    # destructors run at exit), so exactly one copy of the client must exist in
    # a process. EOS builds several shared libraries that are loaded together
    # (e.g. _eos_py.so loads libCCEOSAPI.so and libLanguage_src_shar.so), so
    # they all link libTracyClient instead of embedding the client.
    #
    # libeos.a is the exception: the root CMakeLists.txt merges TracyObjects into
    # it, so that software linking libeos.a gets the client with no extra library.

    find_package(Threads REQUIRED)

    # Compile TracyClient.cpp once as OBJECT library.
    add_library(TracyObjects OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/public/TracyClient.cpp)
    target_include_directories(TracyObjects PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)
    target_compile_definitions(TracyObjects PUBLIC TRACY_ENABLE)
    # No warnings for the Tracy sources.
    target_compile_options(TracyObjects PRIVATE -w)

    if(BUILD_SHARED_LIB)
        add_library(tracy SHARED $<TARGET_OBJECTS:TracyObjects>)
    else()
        add_library(tracy STATIC $<TARGET_OBJECTS:TracyObjects>)
    endif()
    set_target_properties(tracy PROPERTIES OUTPUT_NAME TracyClient)
    # SYSTEM: no warnings from the Tracy headers in the code including them.
    target_include_directories(tracy SYSTEM PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>
        $<INSTALL_INTERFACE:include>)
    target_compile_definitions(tracy PUBLIC TRACY_ENABLE)
    target_link_libraries(tracy PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})

    install(TARGETS tracy
            EXPORT EOSTargets
            LIBRARY DESTINATION ${EOS_LIB_DIR}
            ARCHIVE DESTINATION ${EOS_LIB_DIR})

    message(STATUS "Tracy profiling : ON (merged into libeos.a, libTracyClient for shared libraries)")
else()
    # INTERFACE-only stub: headers available, TRACY_ENABLE not set →
    # all Tracy macros expand to nothing, zero overhead.
    add_library(tracy INTERFACE)
    target_include_directories(tracy INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/public)

    message(STATUS "Tracy profiling : OFF")
endif()
