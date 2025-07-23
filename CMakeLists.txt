cmake_minimum_required(VERSION 3.16)
project(linuxtimelineviewer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable static Qt build
set(BUILD_STATIC ON CACHE BOOL "Build statically against Qt")

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core)

# Collect all source files
file(GLOB_RECURSE SOURCES
    src/*.cpp
)
file(GLOB_RECURSE HEADERS
    src/*.h
)

# Add executable
add_executable(timeline-explorer ${SOURCES} ${HEADERS})

# Link Qt6 libraries
if(BUILD_STATIC)
    set_target_properties(timeline-explorer PROPERTIES WIN32_EXECUTABLE TRUE)
    target_link_libraries(timeline-explorer PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core)
else()
    target_link_libraries(timeline-explorer PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core)
endif()

# Include directories
target_include_directories(timeline-explorer PRIVATE src src/utils)

# Add resources (icons, etc.)
qt_add_resources(timeline-explorer resources/icons.qrc)

# Install rules (optional)
install(TARGETS timeline-explorer DESTINATION bin)

# Notes:
# - To build statically, ensure you have a static Qt6 build and set CMAKE_PREFIX_PATH accordingly.
# - Out-of-source builds are supported (use a separate build/ directory). 