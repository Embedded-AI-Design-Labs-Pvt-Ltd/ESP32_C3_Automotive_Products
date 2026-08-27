# Compile (and upload if an ESP32-C3 COM port is present).
# Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
# Muhammad Samiullah — CTO & Founder. All rights reserved.
$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
Set-Location $Root

python (Join-Path $Root "tools\scripts\gen_arduino_sketch.py")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$cli = Get-Command arduino-cli -ErrorAction SilentlyContinue
if (-not $cli) {
    foreach ($g in @(
        (Join-Path ${env:ProgramFiles} "Arduino CLI\arduino-cli.exe"),
        (Join-Path $env:LOCALAPPDATA "Programs\Arduino CLI\arduino-cli.exe"),
        (Join-Path ${env:ProgramFiles} "Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe")
    )) {
        if (Test-Path $g) { $cli = $g; break }
    }
}
if (-not $cli) {
    Write-Host "arduino-cli not found. Open ports/arduino/AEGW_C3/AEGW_C3.ino in Arduino IDE."
    exit 0
}

$fqbn = "esp32:esp32:esp32c3:CDCOnBoot=cdc"
$sketch = Join-Path $Root "ports\arduino\AEGW_C3"
Write-Host "compile $fqbn"
& $cli compile --fqbn $fqbn $sketch
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$ports = & $cli board list 2>$null
Write-Host $ports
$uploadPort = $null
foreach ($line in ($ports | Out-String) -split "`n") {
    if ($line -match "COM\d+" -and $line -notmatch "AMT|SOL") {
        if ($line -match "(COM\d+)") { $uploadPort = $Matches[1]; break }
    }
}
if (-not $uploadPort) {
    Write-Host "No ESP32-C3 serial port (skipping upload). Plug the board USB, not Intel AMT COM3."
    exit 0
}
Write-Host "upload $uploadPort"
& $cli upload -p $uploadPort --fqbn $fqbn $sketch
exit $LASTEXITCODE
