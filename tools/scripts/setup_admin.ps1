# Elevate and install local toolchain pieces that need rights.
# Re-launches itself with UAC if the current token is not elevated.

$ErrorActionPreference = "Continue"

function Test-Elevated {
    $id = [Security.Principal.WindowsIdentity]::GetCurrent()
    $p = New-Object Security.Principal.WindowsPrincipal($id)
    return $p.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

if (-not (Test-Elevated)) {
    Write-Host "Requesting Administrator elevation (UAC)..."
    $self = $MyInvocation.MyCommand.Path
    Start-Process -FilePath "powershell.exe" -Verb RunAs -ArgumentList @(
        "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $self
    ) | Out-Null
    exit 0
}

Write-Host "Elevated. Installing / repairing tools..."

winget install --id Espressif.EIM-CLI -e --accept-package-agreements --accept-source-agreements --disable-interactivity
winget install --id Python.Python.3.12 -e --accept-package-agreements --accept-source-agreements --disable-interactivity

$docker = "C:\Program Files\Docker\Docker\Docker Desktop.exe"
if (Test-Path $docker) {
    Write-Host "Starting Docker Desktop..."
    Start-Process $docker
} else {
    Write-Host "Docker Desktop not found. Install from winget: Docker.DockerDesktop"
}

Write-Host "Done. ESP-IDF itself is installed later with: eim install"
Write-Host "Press Enter to close."
[void][Console]::ReadLine()
