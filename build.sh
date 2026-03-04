#!/bin/bash
# Zgine Quick Build Script
# Usage: ./build.sh [debug|release|clean]

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Detect OS
OS="unknown"
case "$(uname -s)" in
    Linux*)     OS="linux";;
    Darwin*)    OS="macos";;
    *)          echo -e "${RED}Unsupported OS${NC}"; exit 1;;
esac

# Default build type
BUILD_TYPE="${1:-debug}"

# Function to print colored messages
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check for required tools
check_requirements() {
    print_info "Checking requirements..."
    
    local missing_tools=0
    
    if ! command -v cmake &> /dev/null; then
        print_error "CMake not found. Please install CMake 3.20 or higher."
        missing_tools=1
    fi
    
    if ! command -v git &> /dev/null; then
        print_error "Git not found. Please install Git."
        missing_tools=1
    fi
    
    if ! command -v ninja &> /dev/null; then
        print_warning "Ninja not found. Will use default generator (slower)."
    fi
    
    if [ $missing_tools -ne 0 ]; then
        exit 1
    fi
    
    print_success "All required tools found."
}

# Check vendor dependencies
check_vendor_deps() {
    print_info "Checking vendor dependencies..."
    
    local missing_deps=0
    
    if [ ! -f "vendor/glad/src/glad.c" ]; then
        print_error "GLAD not found in vendor/glad/"
        print_info "Please download GLAD from https://glad.dav1d.de/"
        print_info "Or run: ./scripts/setup_glad.sh"
        missing_deps=1
    fi
    
    if [ ! -f "vendor/stb/stb_image.h" ]; then
        print_error "stb_image not found in vendor/stb/"
        print_info "Downloading stb_image.h..."
        mkdir -p vendor/stb
        if curl -s -o vendor/stb/stb_image.h \
            https://raw.githubusercontent.com/nothings/stb/master/stb_image.h; then
            print_success "Downloaded stb_image.h"
        else
            print_error "Failed to download stb_image.h"
            missing_deps=1
        fi
    fi
    
    if [ $missing_deps -ne 0 ]; then
        exit 1
    fi
    
    print_success "All vendor dependencies found."
}

# Clean build
clean_build() {
    print_info "Cleaning build directories..."
    
    if [ -d "build" ]; then
        rm -rf build
        print_success "Removed build directory"
    fi
    
    if [ -d "bin" ]; then
        rm -rf bin
        print_success "Removed bin directory"
    fi
    
    if [ -d "lib" ]; then
        rm -rf lib
        print_success "Removed lib directory"
    fi
    
    print_success "Clean complete."
}

# Configure build
configure_build() {
    local preset="${OS}-${BUILD_TYPE}"
    
    print_info "Configuring build with preset: ${preset}"
    
    if [ -f "CMakePresets.json" ]; then
        cmake --preset="${preset}"
    else
        print_warning "CMakePresets.json not found, using manual configuration"
        
        mkdir -p "build/${BUILD_TYPE}"
        cd "build/${BUILD_TYPE}"
        
        cmake ../.. \
            -DCMAKE_BUILD_TYPE="${BUILD_TYPE^}" \
            -DZGINE_ENABLE_ASSERTIONS=ON \
            -DZGINE_USE_PCH=ON \
            -G "$(command -v ninja &> /dev/null && echo Ninja || echo 'Unix Makefiles')"
        
        cd ../..
    fi
    
    print_success "Configuration complete."
}

# Build project
build_project() {
    local preset="${OS}-${BUILD_TYPE}"
    
    print_info "Building Zgine (${BUILD_TYPE})..."
    
    # Get CPU count for parallel build
    local cpu_count=1
    if command -v nproc &> /dev/null; then
        cpu_count=$(nproc)
    elif command -v sysctl &> /dev/null; then
        cpu_count=$(sysctl -n hw.ncpu)
    fi
    
    print_info "Using ${cpu_count} parallel jobs"
    
    if [ -f "CMakePresets.json" ]; then
        cmake --build --preset="${preset}" -j "${cpu_count}"
    else
        cmake --build "build/${BUILD_TYPE}" -j "${cpu_count}"
    fi
    
    print_success "Build complete."
}

# Run the executable
run_executable() {
    print_info "Starting Zgine..."
    
    local exe_path=""
    
    if [ -f "build/${OS}-${BUILD_TYPE}/bin/Zgine" ]; then
        exe_path="build/${OS}-${BUILD_TYPE}/bin/Zgine"
    elif [ -f "build/${BUILD_TYPE}/bin/Zgine" ]; then
        exe_path="build/${BUILD_TYPE}/bin/Zgine"
    elif [ -f "build/bin/Zgine" ]; then
        exe_path="build/bin/Zgine"
    else
        print_error "Executable not found!"
        exit 1
    fi
    
    print_info "Executable: ${exe_path}"
    "./${exe_path}"
}

# Main script
main() {
    echo ""
    echo "╔═══════════════════════════════════════╗"
    echo "║     Zgine Build Script v1.0.0         ║"
    echo "╚═══════════════════════════════════════╝"
    echo ""
    
    case "$BUILD_TYPE" in
        clean)
            clean_build
            exit 0
            ;;
        debug|release)
            ;;
        *)
            print_error "Invalid build type: $BUILD_TYPE"
            print_info "Usage: $0 [debug|release|clean]"
            exit 1
            ;;
    esac
    
    check_requirements
    check_vendor_deps
    configure_build
    build_project
    
    print_success "All steps completed successfully!"
    echo ""
    print_info "To run the engine:"
    print_info "  ./build/${OS}-${BUILD_TYPE}/bin/Zgine"
    echo ""
    
    # Ask if user wants to run
    read -p "$(echo -e ${YELLOW}Do you want to run Zgine now? [y/N]:${NC} )" -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        run_executable
    fi
}

# Run main function
main
