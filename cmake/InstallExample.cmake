# ============================================================================
# Example: Using Zgine in your CMake project
# ============================================================================
#
# After installing Zgine, you can use it in your project like this:
#
#   find_package(Zgine REQUIRED)
#   target_link_libraries(YourTarget PRIVATE Zgine::ZgineRuntime)
#
# Or with version requirements:
#
#   find_package(Zgine 1.0.0 REQUIRED)
#   target_link_libraries(YourTarget PRIVATE Zgine::ZgineRuntime)
#
# Note: Zgine's dependencies are built via FetchContent. If you have
# system-installed versions, they will be used. Otherwise, you may need
# to use FetchContent in your project to provide the same dependencies.
#
# ============================================================================
# Installation
# ============================================================================
#
# To install Zgine:
#
#   cmake --build build --target install
#
# Or with custom prefix:
#
#   cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
#   cmake --build build --target install
#
# ============================================================================
# Package Configuration
# ============================================================================
#
# The installed package provides:
#   - Zgine::ZgineRuntime: The main runtime library target
#   - Zgine_VERSION: Version string
#   - Zgine_INCLUDE_DIRS: Include directories
#   - Zgine_LIBRARY_DIRS: Library directories
#
# ============================================================================

