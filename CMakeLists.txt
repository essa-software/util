project(EssaUtil)
cmake_minimum_requiRed(VERSION 3.17)

include(cmake/utils.cmake)

set(CMAKE_CXX_STANDARD 20)

find_package(fmt REQUIRED)

add_library(essautil
    Util/Buffer.cpp
    Util/Color.cpp
    Util/SimulationClock.cpp
    Util/Stream/File.cpp
    Util/Stream/MemoryStream.cpp
    Util/Stream/Reader.cpp
    Util/Stream/StandardStreams.cpp
    Util/Stream/Writer.cpp
    Util/UnitDisplay.cpp
    Util/UString.cpp
)

essautil_setup_target(essautil)
target_link_libraries(essautil fmt)

install(TARGETS essautil EXPORT EssaUtil DESTINATION lib)

install(DIRECTORY cmake/ DESTINATION lib/cmake/EssaUtil)
install(DIRECTORY Util/ DESTINATION include/EssaUtil FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
install(EXPORT EssaUtil DESTINATION lib/cmake/EssaUtil)

add_subdirectory(Tests)
