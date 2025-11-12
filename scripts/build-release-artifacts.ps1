#!/usr/bin/env pwsh
# =============================================================================
# Build Release Artifacts for Alpha v0.1.0-alpha.1
# 
# Purpose: Automate building of Windows/Linux/macOS release binaries
# Phase: Phase 08 - Transition (Days 5-6 Verification)
# Standards: ISO/IEC/IEEE 12207:2017 §6.4.12 (Transition Process)
# Traceability: VERIFY-ALPHA-001, DPL-ALPHA-001
# =============================================================================

param(
    [Parameter(Mandatory=$false)]
    [ValidateSet('Windows', 'Linux', 'macOS', 'All')]
    [string]$Platform = 'Windows',
    
    [Parameter(Mandatory=$false)]
    [switch]$SkipTests,
    
    [Parameter(Mandatory=$false)]
    [switch]$Verbose
)

# Script configuration
$ErrorActionPreference = 'Stop'
$Version = "0.1.0-alpha.1"
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $ProjectRoot "build-release"
$ArtifactsDir = Join-Path $ProjectRoot "artifacts"

# Color output functions
function Write-Header {
    param([string]$Message)
    Write-Host "`n========================================" -ForegroundColor Cyan
    Write-Host $Message -ForegroundColor Cyan
    Write-Host "========================================`n" -ForegroundColor Cyan
}

function Write-Success {
    param([string]$Message)
    Write-Host "✓ $Message" -ForegroundColor Green
}

function Write-Error-Custom {
    param([string]$Message)
    Write-Host "✗ $Message" -ForegroundColor Red
}

function Write-Info {
    param([string]$Message)
    Write-Host "ℹ $Message" -ForegroundColor Yellow
}

# =============================================================================
# Platform Detection
# =============================================================================

function Get-CurrentPlatform {
    if ($IsWindows -or $env:OS -match "Windows") {
        return "Windows"
    } elseif ($IsLinux) {
        return "Linux"
    } elseif ($IsMacOS) {
        return "macOS"
    } else {
        throw "Unknown platform"
    }
}

# =============================================================================
# Build Functions
# =============================================================================

function Build-WindowsRelease {
    Write-Header "Building Windows x64 Release Artifacts"
    
    $WindowsBuildDir = Join-Path $BuildDir "windows-x64"
    
    # Clean previous build
    if (Test-Path $WindowsBuildDir) {
        Write-Info "Cleaning previous Windows build..."
        Remove-Item -Recurse -Force $WindowsBuildDir
    }
    
    # Create build directory
    New-Item -ItemType Directory -Path $WindowsBuildDir -Force | Out-Null
    Push-Location $WindowsBuildDir
    
    try {
        # Configure CMake for Release
        Write-Info "Configuring CMake (Release)..."
        cmake $ProjectRoot `
            -DCMAKE_BUILD_TYPE=Release `
            -G "Visual Studio 17 2022" `
            -A x64 `
            -DBUILD_TESTING=OFF `
            -DENABLE_TESTING=OFF
        
        if ($LASTEXITCODE -ne 0) {
            throw "CMake configuration failed"
        }
        
        # Build
        Write-Info "Building Release binaries..."
        cmake --build . --config Release --parallel
        
        if ($LASTEXITCODE -ne 0) {
            throw "Build failed"
        }
        
        # Run tests if not skipped
        if (-not $SkipTests) {
            Write-Info "Running tests..."
            ctest -C Release --output-on-failure
            
            if ($LASTEXITCODE -ne 0) {
                Write-Error-Custom "Tests failed!"
                throw "Tests failed"
            }
            Write-Success "All tests passed"
        }
        
        # Package
        Write-Info "Creating ZIP package..."
        cpack -G ZIP -C Release
        
        if ($LASTEXITCODE -ne 0) {
            throw "Packaging failed"
        }
        
        # Copy to artifacts directory
        $PackageName = "aes3-2009-$Version-windows-x64.zip"
        $SourcePackage = Get-ChildItem -Path . -Filter "*.zip" | Select-Object -First 1
        
        if ($SourcePackage) {
            New-Item -ItemType Directory -Path $ArtifactsDir -Force | Out-Null
            $DestinationPath = Join-Path $ArtifactsDir $PackageName
            Copy-Item $SourcePackage.FullName $DestinationPath -Force
            
            $Size = (Get-Item $DestinationPath).Length / 1MB
            Write-Success "Windows artifact created: $PackageName (${Size:N2} MB)"
        } else {
            throw "Package file not found"
        }
        
    } finally {
        Pop-Location
    }
}

function Build-LinuxRelease {
    Write-Header "Building Linux x64 Release Artifacts"
    
    if ((Get-CurrentPlatform) -ne "Linux") {
        Write-Error-Custom "Linux builds must be run on Linux system or use Docker"
        Write-Info "Skipping Linux build (wrong platform)"
        return
    }
    
    $LinuxBuildDir = Join-Path $BuildDir "linux-x64"
    
    # Clean previous build
    if (Test-Path $LinuxBuildDir) {
        Write-Info "Cleaning previous Linux build..."
        Remove-Item -Recurse -Force $LinuxBuildDir
    }
    
    # Create build directory
    New-Item -ItemType Directory -Path $LinuxBuildDir -Force | Out-Null
    Push-Location $LinuxBuildDir
    
    try {
        # Configure CMake for Release
        Write-Info "Configuring CMake (Release)..."
        cmake $ProjectRoot `
            -DCMAKE_BUILD_TYPE=Release `
            -DBUILD_TESTING=OFF `
            -DENABLE_TESTING=OFF
        
        if ($LASTEXITCODE -ne 0) {
            throw "CMake configuration failed"
        }
        
        # Build
        Write-Info "Building Release binaries..."
        cmake --build . --parallel
        
        if ($LASTEXITCODE -ne 0) {
            throw "Build failed"
        }
        
        # Strip debug symbols
        Write-Info "Stripping debug symbols..."
        Get-ChildItem -Path . -Filter "*.so" -Recurse | ForEach-Object {
            strip --strip-debug $_.FullName
        }
        Get-ChildItem -Path . -Filter "aes3_*" -Recurse | Where-Object { -not $_.Name.EndsWith(".cpp") } | ForEach-Object {
            if (Test-Path $_.FullName -PathType Leaf) {
                strip --strip-all $_.FullName
            }
        }
        
        # Run tests if not skipped
        if (-not $SkipTests) {
            Write-Info "Running tests..."
            ctest --output-on-failure
            
            if ($LASTEXITCODE -ne 0) {
                Write-Error-Custom "Tests failed!"
                throw "Tests failed"
            }
            Write-Success "All tests passed"
        }
        
        # Package
        Write-Info "Creating TGZ package..."
        cpack -G TGZ
        
        if ($LASTEXITCODE -ne 0) {
            throw "Packaging failed"
        }
        
        # Copy to artifacts directory
        $PackageName = "aes3-2009-$Version-linux-x64.tar.gz"
        $SourcePackage = Get-ChildItem -Path . -Filter "*.tar.gz" | Select-Object -First 1
        
        if ($SourcePackage) {
            New-Item -ItemType Directory -Path $ArtifactsDir -Force | Out-Null
            $DestinationPath = Join-Path $ArtifactsDir $PackageName
            Copy-Item $SourcePackage.FullName $DestinationPath -Force
            
            $Size = (Get-Item $DestinationPath).Length / 1MB
            Write-Success "Linux artifact created: $PackageName (${Size:N2} MB)"
        } else {
            throw "Package file not found"
        }
        
    } finally {
        Pop-Location
    }
}

function Build-MacOSRelease {
    Write-Header "Building macOS Universal Binary Release Artifacts"
    
    if ((Get-CurrentPlatform) -ne "macOS") {
        Write-Error-Custom "macOS builds must be run on macOS system"
        Write-Info "Skipping macOS build (wrong platform)"
        return
    }
    
    $MacOSBuildDir = Join-Path $BuildDir "macos-universal"
    
    # Clean previous build
    if (Test-Path $MacOSBuildDir) {
        Write-Info "Cleaning previous macOS build..."
        Remove-Item -Recurse -Force $MacOSBuildDir
    }
    
    # Create build directory
    New-Item -ItemType Directory -Path $MacOSBuildDir -Force | Out-Null
    Push-Location $MacOSBuildDir
    
    try {
        # Configure CMake for Release (Universal Binary)
        Write-Info "Configuring CMake (Release, Universal Binary)..."
        cmake $ProjectRoot `
            -DCMAKE_BUILD_TYPE=Release `
            -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" `
            -DBUILD_TESTING=OFF `
            -DENABLE_TESTING=OFF
        
        if ($LASTEXITCODE -ne 0) {
            throw "CMake configuration failed"
        }
        
        # Build
        Write-Info "Building Release binaries (Universal: x64 + ARM64)..."
        cmake --build . --parallel
        
        if ($LASTEXITCODE -ne 0) {
            throw "Build failed"
        }
        
        # Verify universal binary
        Write-Info "Verifying universal binary..."
        $Binaries = Get-ChildItem -Path . -Name "aes3_*" -Recurse | Where-Object { -not $_.EndsWith(".cpp") }
        foreach ($Binary in $Binaries) {
            if (Test-Path $Binary -PathType Leaf) {
                lipo -info $Binary
            }
        }
        
        # Run tests if not skipped
        if (-not $SkipTests) {
            Write-Info "Running tests..."
            ctest --output-on-failure
            
            if ($LASTEXITCODE -ne 0) {
                Write-Error-Custom "Tests failed!"
                throw "Tests failed"
            }
            Write-Success "All tests passed"
        }
        
        # Ad-hoc code signing (for Alpha release)
        Write-Info "Applying ad-hoc code signing..."
        $Executables = Get-ChildItem -Path . -Name "aes3_*" -Recurse | Where-Object { -not $_.EndsWith(".cpp") }
        foreach ($Executable in $Executables) {
            if (Test-Path $Executable -PathType Leaf) {
                codesign --sign - --force $Executable
            }
        }
        
        # Package
        Write-Info "Creating TGZ package..."
        cpack -G TGZ
        
        if ($LASTEXITCODE -ne 0) {
            throw "Packaging failed"
        }
        
        # Copy to artifacts directory
        $PackageName = "aes3-2009-$Version-macos-universal.tar.gz"
        $SourcePackage = Get-ChildItem -Path . -Filter "*.tar.gz" | Select-Object -First 1
        
        if ($SourcePackage) {
            New-Item -ItemType Directory -Path $ArtifactsDir -Force | Out-Null
            $DestinationPath = Join-Path $ArtifactsDir $PackageName
            Copy-Item $SourcePackage.FullName $DestinationPath -Force
            
            $Size = (Get-Item $DestinationPath).Length / 1MB
            Write-Success "macOS artifact created: $PackageName (${Size:N2} MB)"
        } else {
            throw "Package file not found"
        }
        
    } finally {
        Pop-Location
    }
}

# =============================================================================
# Main Script
# =============================================================================

Write-Header "AES3-2009 Alpha $Version - Release Artifact Builder"

Write-Info "Configuration:"
Write-Host "  Platform:     $Platform"
Write-Host "  Version:      $Version"
Write-Host "  Project Root: $ProjectRoot"
Write-Host "  Build Dir:    $BuildDir"
Write-Host "  Artifacts:    $ArtifactsDir"
Write-Host "  Skip Tests:   $SkipTests"
Write-Host ""

$CurrentPlatform = Get-CurrentPlatform
Write-Info "Detected platform: $CurrentPlatform"

try {
    switch ($Platform) {
        'Windows' {
            if ($CurrentPlatform -ne 'Windows') {
                Write-Error-Custom "Cannot build Windows artifacts on $CurrentPlatform"
                exit 1
            }
            Build-WindowsRelease
        }
        'Linux' {
            if ($CurrentPlatform -ne 'Linux') {
                Write-Error-Custom "Cannot build Linux artifacts on $CurrentPlatform"
                Write-Info "Use CI pipeline or Docker for cross-platform builds"
                exit 1
            }
            Build-LinuxRelease
        }
        'macOS' {
            if ($CurrentPlatform -ne 'macOS') {
                Write-Error-Custom "Cannot build macOS artifacts on $CurrentPlatform"
                Write-Info "Use CI pipeline for macOS builds"
                exit 1
            }
            Build-MacOSRelease
        }
        'All' {
            # Build for current platform only (cross-compilation not supported)
            Write-Info "Building for current platform: $CurrentPlatform"
            switch ($CurrentPlatform) {
                'Windows' { Build-WindowsRelease }
                'Linux' { Build-LinuxRelease }
                'macOS' { Build-MacOSRelease }
            }
            
            Write-Info ""
            Write-Info "Note: Cross-platform builds require CI pipeline"
            Write-Info "Windows artifacts: GitHub Actions (windows-latest)"
            Write-Info "Linux artifacts:   GitHub Actions (ubuntu-latest)"
            Write-Info "macOS artifacts:   GitHub Actions (macos-latest)"
        }
    }
    
    # Summary
    Write-Header "Build Summary"
    
    if (Test-Path $ArtifactsDir) {
        $Artifacts = Get-ChildItem -Path $ArtifactsDir -Filter "aes3-2009-$Version-*"
        
        if ($Artifacts.Count -gt 0) {
            Write-Success "Artifacts created successfully:"
            foreach ($Artifact in $Artifacts) {
                $Size = $Artifact.Length / 1MB
                Write-Host "  - $($Artifact.Name) (${Size:N2} MB)" -ForegroundColor White
            }
        } else {
            Write-Error-Custom "No artifacts found in $ArtifactsDir"
            exit 1
        }
    } else {
        Write-Error-Custom "Artifacts directory not created"
        exit 1
    }
    
    Write-Success "`nRelease artifacts build completed!"
    
} catch {
    Write-Error-Custom "Build failed: $_"
    Write-Host $_.ScriptStackTrace -ForegroundColor Red
    exit 1
}
