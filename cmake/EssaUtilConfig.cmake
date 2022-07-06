if (ESSAUTIL_ENABLE_SFML)
    find_package(SFML 2.5.1 COMPONENTS system REQUIRED)
endif()
include(${CMAKE_CURRENT_LIST_DIR}/EssaGUI.cmake)
