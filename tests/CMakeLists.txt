cmake_minimum_required(VERSION 3.20)

# Zgine tests application
project(ZgineTests VERSION 1.0.0 LANGUAGES CXX)

# Enable testing
enable_testing()

# Find or use parent's ZgineRuntime target
if(NOT TARGET ZgineRuntime)
    # If built standalone, find installed Zgine
    find_package(Zgine REQUIRED)
    find_package(GTest REQUIRED)
    # ZgineRuntime is exported as Zgine::ZgineRuntime
    set(ZgineRuntime Zgine::ZgineRuntime)
else()
    # If built as part of Zgine project, use the target directly
    message(STATUS "Using ZgineRuntime from parent project")
endif()

# Test executable
add_executable(ZgineTests
    AssetManagerTests.cpp
    EngineSmokeTests.cpp
    RendererBackendTests.cpp
    SystemManagerTests.cpp
)

# Link to ZgineRuntime and GoogleTest
target_link_libraries(ZgineTests PRIVATE
    ZgineRuntime
    GTest::gtest_main
)

# C++ standard
set_target_properties(ZgineTests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(ZgineTests
    DISCOVERY_TIMEOUT 60
    DISCOVERY_MODE PRE_TEST
)

# Add Editor tests if enabled
if(ZGINE_BUILD_EDITOR)
    add_subdirectory(Editor)
endif()
