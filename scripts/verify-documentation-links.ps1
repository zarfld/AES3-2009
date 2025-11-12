#!/usr/bin/env pwsh
# =============================================================================
# Verify Documentation Links
# 
# Purpose: Automated verification of all documentation links
# Phase: Phase 08 - Transition (Days 5-6 Verification)
# Standards: ISO/IEC/IEEE 12207:2017 §6.4.12 (Transition Process)
# Traceability: VERIFY-ALPHA-001, DPL-ALPHA-001
# =============================================================================

param(
    [Parameter(Mandatory=$false)]
    [switch]$Verbose,
    
    [Parameter(Mandatory=$false)]
    [switch]$FailFast
)

$ErrorActionPreference = 'Continue'
$ProjectRoot = Split-Path -Parent $PSScriptRoot

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

function Write-Warning-Custom {
    param([string]$Message)
    Write-Host "⚠ $Message" -ForegroundColor Yellow
}

function Write-Info {
    param([string]$Message)
    Write-Host "ℹ $Message" -ForegroundColor Yellow
}

# =============================================================================
# Link Verification Functions
# =============================================================================

function Test-LocalPath {
    param(
        [string]$Path,
        [string]$SourceFile
    )
    
    # Resolve relative path from source file location
    $SourceDir = Split-Path -Parent $SourceFile
    $FullPath = Join-Path $SourceDir $Path
    
    # Try alternative: relative to project root
    if (-not (Test-Path $FullPath)) {
        $FullPath = Join-Path $ProjectRoot $Path
    }
    
    if (Test-Path $FullPath) {
        return @{ Success = $true; Path = $FullPath }
    } else {
        return @{ Success = $false; Path = $FullPath; Error = "File not found" }
    }
}

function Test-WebUrl {
    param(
        [string]$Url
    )
    
    try {
        $Response = Invoke-WebRequest -Uri $Url -Method Head -TimeoutSec 10 -UseBasicParsing
        return @{ Success = $true; StatusCode = $Response.StatusCode }
    } catch {
        return @{ Success = $false; Error = $_.Exception.Message }
    }
}

function Get-MarkdownLinks {
    param(
        [string]$FilePath
    )
    
    $Content = Get-Content -Path $FilePath -Raw
    
    # Regex patterns for different link types
    $Patterns = @(
        # Markdown links: [text](url)
        '\[([^\]]+)\]\(([^\)]+)\)',
        # Reference-style links: [text][ref]
        '\[([^\]]+)\]\[([^\]]+)\]',
        # Bare URLs (http/https)
        '(?<![\[\(])https?://[^\s\)\]>]+'
    )
    
    $Links = @()
    
    foreach ($Pattern in $Patterns) {
        $Matches = [regex]::Matches($Content, $Pattern)
        foreach ($Match in $Matches) {
            if ($Match.Groups.Count -eq 3) {
                # Standard markdown link [text](url)
                $Links += @{
                    Text = $Match.Groups[1].Value
                    Url = $Match.Groups[2].Value
                    Line = ($Content.Substring(0, $Match.Index) -split "`n").Count
                }
            } elseif ($Match.Groups.Count -eq 1) {
                # Bare URL
                $Links += @{
                    Text = "(bare URL)"
                    Url = $Match.Value
                    Line = ($Content.Substring(0, $Match.Index) -split "`n").Count
                }
            }
        }
    }
    
    return $Links
}

function Test-DocumentLinks {
    param(
        [string]$FilePath
    )
    
    Write-Info "Checking: $($FilePath.Replace($ProjectRoot, '.'))"
    
    $Links = Get-MarkdownLinks -FilePath $FilePath
    $Results = @{
        Total = 0
        Passed = 0
        Failed = 0
        Warnings = 0
        Details = @()
    }
    
    foreach ($Link in $Links) {
        $Results.Total++
        
        $Url = $Link.Url
        
        # Skip anchors and mailto
        if ($Url.StartsWith('#') -or $Url.StartsWith('mailto:')) {
            if ($Verbose) {
                Write-Host "  ⊘ Skipped: $Url (anchor/mailto)" -ForegroundColor DarkGray
            }
            continue
        }
        
        # Determine link type
        if ($Url -match '^https?://') {
            # Web URL
            $Result = Test-WebUrl -Url $Url
            
            if ($Result.Success) {
                $Results.Passed++
                if ($Verbose) {
                    Write-Host "  ✓ OK: $Url (HTTP $($Result.StatusCode))" -ForegroundColor Green
                }
            } else {
                $Results.Failed++
                Write-Error-Custom "  ✗ FAILED: $Url"
                Write-Host "    Error: $($Result.Error)" -ForegroundColor Red
                Write-Host "    Line $($Link.Line): $($Link.Text)" -ForegroundColor DarkGray
                
                $Results.Details += @{
                    File = $FilePath
                    Line = $Link.Line
                    Link = $Url
                    Error = $Result.Error
                    Type = "Web"
                }
                
                if ($FailFast) {
                    throw "Link verification failed (fail-fast enabled)"
                }
            }
        } else {
            # Local path
            $Result = Test-LocalPath -Path $Url -SourceFile $FilePath
            
            if ($Result.Success) {
                $Results.Passed++
                if ($Verbose) {
                    Write-Host "  ✓ OK: $Url" -ForegroundColor Green
                }
            } else {
                # Check if it might be a GitHub URL pattern
                if ($Url -match '^(github\.com|raw\.githubusercontent\.com)') {
                    $Results.Warnings++
                    Write-Warning-Custom "  ⚠ WARNING: Possible GitHub URL without protocol: $Url"
                    $Results.Details += @{
                        File = $FilePath
                        Line = $Link.Line
                        Link = $Url
                        Error = "Missing https:// protocol?"
                        Type = "Warning"
                    }
                } else {
                    $Results.Failed++
                    Write-Error-Custom "  ✗ FAILED: $Url"
                    Write-Host "    Error: $($Result.Error)" -ForegroundColor Red
                    Write-Host "    Searched: $($Result.Path)" -ForegroundColor DarkGray
                    Write-Host "    Line $($Link.Line): $($Link.Text)" -ForegroundColor DarkGray
                    
                    $Results.Details += @{
                        File = $FilePath
                        Line = $Link.Line
                        Link = $Url
                        Error = $Result.Error
                        Type = "Local"
                    }
                    
                    if ($FailFast) {
                        throw "Link verification failed (fail-fast enabled)"
                    }
                }
            }
        }
    }
    
    return $Results
}

# =============================================================================
# Main Script
# =============================================================================

Write-Header "AES3-2009 Documentation Link Verification"

Write-Info "Configuration:"
Write-Host "  Project Root: $ProjectRoot"
Write-Host "  Verbose:      $Verbose"
Write-Host "  Fail Fast:    $FailFast"
Write-Host ""

# Documents to verify (per VERIFY-ALPHA-001)
$Documents = @(
    "README.md",
    "ALPHA_TESTING_GUIDE.md",
    "CONTRIBUTING.md",
    "CODE_OF_CONDUCT.md",
    "08-transition/deployment-plans/alpha-release-plan.md",
    "08-transition/deployment-plans/alpha-release-announcement.md",
    "08-transition/deployment-plans/github-setup-checklist.md",
    "08-transition/deployment-plans/pre-release-verification-checklist.md",
    "field-reliability-report-template.md",
    ".github/ISSUE_TEMPLATE/bug_report.md",
    ".github/ISSUE_TEMPLATE/equipment_compatibility.md",
    ".github/ISSUE_TEMPLATE/feature_request.md",
    ".github/ISSUE_TEMPLATE/test_results.md",
    ".github/ISSUE_TEMPLATE/config.yml"
)

$TotalResults = @{
    Files = 0
    Total = 0
    Passed = 0
    Failed = 0
    Warnings = 0
    FailedFiles = @()
}

foreach ($Document in $Documents) {
    $FilePath = Join-Path $ProjectRoot $Document
    
    if (-not (Test-Path $FilePath)) {
        Write-Warning-Custom "Document not found: $Document"
        continue
    }
    
    try {
        $Results = Test-DocumentLinks -FilePath $FilePath
        
        $TotalResults.Files++
        $TotalResults.Total += $Results.Total
        $TotalResults.Passed += $Results.Passed
        $TotalResults.Failed += $Results.Failed
        $TotalResults.Warnings += $Results.Warnings
        
        if ($Results.Failed -gt 0) {
            $TotalResults.FailedFiles += $Document
        }
        
        # Summary for this file
        if ($Results.Failed -eq 0 -and $Results.Warnings -eq 0) {
            Write-Success "  ✓ All links verified ($($Results.Passed)/$($Results.Total))"
        } elseif ($Results.Failed -gt 0) {
            Write-Error-Custom "  ✗ $($Results.Failed) failed, $($Results.Passed) passed, $($Results.Warnings) warnings"
        } else {
            Write-Warning-Custom "  ⚠ $($Results.Warnings) warnings, $($Results.Passed) passed"
        }
        
    } catch {
        Write-Error-Custom "Error processing $Document : $_"
        $TotalResults.FailedFiles += $Document
    }
    
    Write-Host ""
}

# =============================================================================
# Summary Report
# =============================================================================

Write-Header "Verification Summary"

Write-Host "Documents Checked: $($TotalResults.Files)"
Write-Host "Total Links:       $($TotalResults.Total)"
Write-Host ""

if ($TotalResults.Passed -gt 0) {
    Write-Success "Passed:  $($TotalResults.Passed)"
}

if ($TotalResults.Failed -gt 0) {
    Write-Error-Custom "Failed:  $($TotalResults.Failed)"
}

if ($TotalResults.Warnings -gt 0) {
    Write-Warning-Custom "Warnings: $($TotalResults.Warnings)"
}

Write-Host ""

# Exit code
if ($TotalResults.Failed -gt 0) {
    Write-Error-Custom "`n✗ Documentation link verification FAILED"
    Write-Host "Failed documents:" -ForegroundColor Red
    foreach ($File in $TotalResults.FailedFiles) {
        Write-Host "  - $File" -ForegroundColor Red
    }
    exit 1
} elseif ($TotalResults.Warnings -gt 0) {
    Write-Warning-Custom "`n⚠ Documentation link verification completed with WARNINGS"
    exit 0
} else {
    Write-Success "`n✓ All documentation links verified successfully!"
    exit 0
}
