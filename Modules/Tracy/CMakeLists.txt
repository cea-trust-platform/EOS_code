



option(TRACY_ENABLE "Enable Tracy profiling" ON)

add_library(tracy STATIC public/TracyClient.cpp)


target_include_directories(tracy PUBLIC public)


if(TRACY_ENABLE)
    target_compile_definitions(tracy PUBLIC TRACY_ENABLE)
    message(STATUS "Tracy profiling : ON")
else()
    message(STATUS "Tracy profiling : OFF")
endif()