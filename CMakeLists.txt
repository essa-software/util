project(EssaUtil)
cmake_minimum_requiRed(VERSION 3.17)

include(cmake/utils.cmake)

set(CMAKE_CXX_STANDARD 20)

option(ESSAUTIL_ENABLE_SFML "Enable SFML integration (e.g vector converting)" OFF)

add_library(essautil
    Util/Color.cpp
    Util/SimulationClock.cpp
    Util/Stream/File.cpp
    Util/Stream/Reader.cpp
    Util/Stream/StandardStreams.cpp
    Util/Stream/Writer.cpp
    Util/UnitDisplay.cpp
    Util/UString.cpp
)

essautil_setup_target(essautil)
if (ESSAUTIL_ENABLE_SFML)
    find_package(SFML 2.5.1 COMPONENTS system REQUIRED)
    target_compile_definitions(essautil PUBLIC ESSAUTIL_ENABLE_SFML)
    target_link_libraries(essautil sfml-system)
endif()

install(TARGETS essautil EXPORT EssaUtil DESTINATION lib)

install(DIRECTORY cmake/ DESTINATION lib/cmake/EssaUtil)
install(DIRECTORY Util/ DESTINATION include/EssaUtil FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
install(EXPORT EssaUtil DESTINATION lib/cmake/EssaUtil)

add_subdirectory(Tests)
