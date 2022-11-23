project(EssaUtil)
cmake_minimum_required(VERSION 3.17)

include(cmake/utils.cmake)

set(CMAKE_CXX_STANDARD 20)

find_package(fmt 9.1.0 REQUIRED)

add_library(essautil SHARED
    Util/Buffer.cpp
    Util/Color.cpp
    Util/DisplayError.cpp
    Util/Math/Plane.cpp
    Util/Math/Ray.cpp
    Util/SimulationClock.cpp
    Util/Stream/File.cpp
    Util/Stream/MemoryStream.cpp
    Util/Stream/Reader.cpp
    Util/Stream/StandardStreams.cpp
    Util/Stream/Writer.cpp
    Util/UnitDisplay.cpp
    Util/UString.cpp
    Util/UStringBuilder.cpp
)

essautil_setup_target(essautil)
target_link_libraries(essautil fmt::fmt)

install(TARGETS essautil EXPORT EssaUtil DESTINATION lib)

install(DIRECTORY cmake/ DESTINATION lib/cmake/EssaUtil)
install(DIRECTORY Util/ DESTINATION include/EssaUtil FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
install(EXPORT EssaUtil DESTINATION lib/cmake/EssaUtil)

add_subdirectory(Tests)
