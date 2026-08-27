# Elevate and install / repair toolchain pieces that need Administrator rights.
# Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
# Muhammad Samiullah — CTO & Founder. All rights reserved.
#
# Usage:
#   powershell -NoProfile -ExecutionPolicy Bypass -File tools\scripts\setup_admin.ps1
#   powershell -NoProfile -ExecutionPolicy Bypass -File tools\scripts\setup_admin.ps1 -Quiet
#
# -Quiet skips the "Press Enter" pause (used by automation).

param(
    [switch]$Quiet
)

$ErrorActionPreference = "Continue"
$Log = Join-Path $env:TEMP "aegw_admin_setup.log"

function Test-Elevated {
    $id = [Security.Principal.WindowsIdentity]::GetCurrent()
    $p = New-Object Security.Principal.WindowsPrincipal($id)
    return $p.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

function Write-Log([string]$msg) {
    $line = "{0}  {1}" -f (Get-Date -Format "yyyy-MM-dd HH:mm:ss"), $msg
    Write-Host $line
    Add-Content -Path $Log -Value $line
}

if (-not (Test-Elevated)) {
    Write-Host "Requesting Administrator elevation (UAC)..."
    Write-Host "Approve the Windows prompt, then this window waits for the elevated setup to finish."
    $self = $MyInvocation.MyCommand.Path
    $arg = @(
        "-NoProfile", "-ExecutionPolicy", "Bypass",
        "-File", $self
    )
    if ($Quiet) { $arg += "-Quiet" }
    $p = Start-Process -FilePath "powershell.exe" -Verb RunAs -Wait -PassThru -ArgumentList $arg
    if ($null -eq $p) {
        Write-Host "UAC was cancelled or elevation failed."
        exit 1
    }
    Write-Host "Elevated setup exit code: $($p.ExitCode)"
    if (Test-Path $Log) {
        Write-Host "`n--- elevated log ($Log) ---"
        Get-Content $Log
    }
    exit $p.ExitCode
}

Set-Content -Path $Log -Value ""
Write-Log "Elevated as $([Security.Principal.WindowsIdentity]::GetCurrent().Name)"
Write-Log "Installing / repairing Administrator-scope tools..."

function Invoke-Winget([string]$id) {
    Write-Log "winget install $id"
    winget install --id $id -e --accept-package-agreements --accept-source-agreements --disable-interactivity
    Write-Log "winget $id exit $LASTEXITCODE"
}

Invoke-Winget "Espressif.EIM-CLI"
Invoke-Winget "Python.Python.3.12"
Invoke-Winget "ArduinoSA.CLI"
Invoke-Winget "Kitware.CMake"

$eim = Get-Command eim -ErrorAction SilentlyContinue
if (-not $eim) {
    $eimPath = Join-Path $env:LOCALAPPDATA "Microsoft\WinGet\Links\eim.exe"
    if (Test-Path $eimPath) { $eim = $eimPath }
}
if ($eim) {
    Write-Log "eim install-drivers (USB serial for ESP32)"
    & $eim install-drivers --do-not-track true
    Write-Log "eim install-drivers exit $LASTEXITCODE"
    Write-Log "eim list"
    & $eim list
} else {
    Write-Log "eim not on PATH after winget; skip drivers"
}

$docker = "C:\Program Files\Docker\Docker\Docker Desktop.exe"
if (Test-Path $docker) {
    Write-Log "Starting Docker Desktop..."
    Start-Process $docker
} else {
    Write-Log "Docker Desktop not found. Optional: winget install Docker.DockerDesktop"
}

Write-Log "Done. ESP-IDF (if installed): eim run `"idf.py --version`" v5.5.1"
if (-not $Quiet) {
    Write-Host "Press Enter to close."
    [void][Console]::ReadLine()
}
exit 0
