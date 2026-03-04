# Zgine Quick Build Script (Windows PowerShell)
# Usage: .\build.ps1 [debug|release|clean]

param(
    [Parameter(Position=0)]
    [ValidateSet('debug', 'release', 'clean')]
    [string]$BuildType = 'debug'
)

# Error handling
$ErrorActionPreference = "Stop"

# Colors for output
function Write-ColorOutput {
    param(
        [string]$Message,
        [string]$Color = "White"
    )
    Write-Host $Message -ForegroundColor $Color
}

function Print-Info {
    param([string]$Message)
    Write-ColorOutput "[INFO] $Message" "Cyan"
}

function Print-Success {
    param([string]$Message)
    Write-ColorOutput "[SUCCESS] $Message" "Green"
}

function Print-Warning {
    param([string]$Message)
    Write-ColorOutput "[WARNING] $Message" "Yellow"
}

function Print-Error {
    param([string]$Message)
    Write-ColorOutput "[ERROR] $Message" "Red"
}

# Check for required tools
function Test-Requirements {
    Print-Info "Checking requirements..."
    
    $missingTools = 0
    
    # Check CMake
    if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
        Print-Error "CMake not found. Please install CMake 3.20 or higher."
        $missingTools++
    }
    
    # Check Git
    if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
        Print-Error "Git not found. Please install Git."
        $missingTools++
    }
    
    # Check Ninja (optional)
    if (-not (Get-Command ninja -ErrorAction SilentlyContinue)) {
        Print-Warning "Ninja not found. Will use MSBuild (slower)."
    }
    
    # Check Visual Studio
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vsWhere) {
        $vsPath = & $vsWhere -latest -property installationPath
        if ($vsPath) {
            Print-Success "Found Visual Studio at: $vsPath"
        }
    } else {
        Print-Warning "Visual Studio not detected. Ensure you have VS 2019 or later."
    }
    
    if ($missingTools -gt 0) {
        exit 1
    }
    
    Print-Success "All required tools found."
}

# Check vendor dependencies
function Test-VendorDeps {
    Print-Info "Checking vendor dependencies..."
    
    $missingDeps = 0
    
    # Check GLAD
    if (-not (Test-Path "vendor\glad\src\glad.c")) {
        Print-Error "GLAD not found in vendor\glad\"
        Print-Info "Please download GLAD from https://glad.dav1d.de/"
        Print-Info "Or run: .\scripts\setup_glad.ps1"
        $missingDeps++
    }
    
    # Check stb_image
    if (-not (Test-Path "vendor\stb\stb_image.h")) {
        Print-Error "stb_image not found in vendor\stb\"
        Print-Info "Downloading stb_image.h..."
        
        New-Item -ItemType Directory -Force -Path "vendor\stb" | Out-Null
        
        try {
            Invoke-WebRequest `
                -Uri "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h" `
                -OutFile "vendor\stb\stb_image.h"
            Print-Success "Downloaded stb_image.h"
        } catch {
            Print-Error "Failed to download stb_image.h: $_"
            $missingDeps++
        }
    }
    
    if ($missingDeps -gt 0) {
        exit 1
    }
    
    Print-Success "All vendor dependencies found."
}

# Clean build
function Invoke-CleanBuild {
    Print-Info "Cleaning build directories..."
    
    $dirsToRemove = @("build", "bin", "lib")
    
    foreach ($dir in $dirsToRemove) {
        if (Test-Path $dir) {
            Remove-Item -Recurse -Force $dir
            Print-Success "Removed $dir directory"
        }
    }
    
    Print-Success "Clean complete."
}

# Configure build
function Invoke-Configure {
    param([string]$Type)
    
    $preset = "windows-$Type"
    
    Print-Info "Configuring build with preset: $preset"
    
    if (Test-Path "CMakePresets.json") {
        cmake --preset=$preset
    } else {
        Print-Warning "CMakePresets.json not found, using manual configuration"
        
        $buildDir = "build\$Type"
        New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
        
        Push-Location $buildDir
        
        $generator = "Ninja"
        if (-not (Get-Command ninja -ErrorAction SilentlyContinue)) {
            $generator = "Visual Studio 17 2022"
        }
        
        cmake ..\.. `
            -DCMAKE_BUILD_TYPE="$($Type.Substring(0,1).ToUpper())$($Type.Substring(1))" `
            -DZGINE_ENABLE_ASSERTIONS=ON `
            -DZGINE_USE_PCH=ON `
            -G "$generator" `
            -A x64
        
        Pop-Location
    }
    
    Print-Success "Configuration complete."
}

# Build project
function Invoke-Build {
    param([string]$Type)
    
    $preset = "windows-$Type"
    
    Print-Info "Building Zgine ($Type)..."
    
    # Get CPU count for parallel build
    $cpuCount = (Get-CimInstance Win32_ComputerSystem).NumberOfLogicalProcessors
    Print-Info "Using $cpuCount parallel jobs"
    
    if (Test-Path "CMakePresets.json") {
        cmake --build --preset=$preset --parallel $cpuCount
    } else {
        cmake --build "build\$Type" --config $Type --parallel $cpuCount
    }
    
    Print-Success "Build complete."
}

# Run the executable
function Invoke-Run {
    param([string]$Type)
    
    Print-Info "Starting Zgine..."
    
    $exePaths = @(
        "build\windows-$Type\bin\Zgine.exe",
        "build\$Type\bin\$Type\Zgine.exe",
        "build\$Type\bin\Zgine.exe",
        "build\bin\Zgine.exe"
    )
    
    $exePath = $null
    foreach ($path in $exePaths) {
        if (Test-Path $path) {
            $exePath = $path
            break
        }
    }
    
    if (-not $exePath) {
        Print-Error "Executable not found!"
        exit 1
    }
    
    Print-Info "Executable: $exePath"
    & $exePath
}

# Main script
function Main {
    Write-Host ""
    Write-Host "╔═══════════════════════════════════════╗" -ForegroundColor Cyan
    Write-Host "║     Zgine Build Script v1.0.0         ║" -ForegroundColor Cyan
    Write-Host "╚═══════════════════════════════════════╝" -ForegroundColor Cyan
    Write-Host ""
    
    if ($BuildType -eq 'clean') {
        Invoke-CleanBuild
        return
    }
    
    Test-Requirements
    Test-VendorDeps
    Invoke-Configure -Type $BuildType
    Invoke-Build -Type $BuildType
    
    Print-Success "All steps completed successfully!"
    Write-Host ""
    Print-Info "To run the engine:"
    Print-Info "  .\build\windows-$BuildType\bin\Zgine.exe"
    Write-Host ""
    
    # Ask if user wants to run
    $response = Read-Host "Do you want to run Zgine now? [y/N]"
    if ($response -match "^[Yy]") {
        Invoke-Run -Type $BuildType
    }
}

# Run main function
try {
    Main
} catch {
    Print-Error "Build failed: $_"
    exit 1
}
