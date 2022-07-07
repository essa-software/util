function(essautil_setup_target targetname)
    target_compile_options(${targetname} PRIVATE -Wall -Wextra -Werror)
    target_compile_options(${targetname} PRIVATE
        -Wnon-virtual-dtor
        -Wno-error=missing-field-initializers       # FIXME: This is buggy (?) for *DrawOptions
        -Wno-error=format                           # FIXME: We don't use formatting anyway except SimulationClock but this breaks mingw build
    )

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        target_compile_options(${targetname} PUBLIC -fsanitize=undefined,address)
        target_link_options(${targetname} PUBLIC -fsanitize=undefined,address)
    else()
        target_compile_options(${targetname} PUBLIC -O3)
    endif()

    target_include_directories(${targetname} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
endfunction()

function(essautil_add_test targetname)
    cmake_parse_arguments(PARSE_ARGV 1 essautil_add_test "" "" "LIBS")
    add_executable(${targetname} ${targetname}.cpp)
    target_link_libraries(${targetname} ${essautil_add_test_LIBS})
    essautil_setup_target(${targetname})
endfunction()
