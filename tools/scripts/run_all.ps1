# Run host verification: docs + all product use-cases + C++ ports.
# Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
# Muhammad Samiullah — CTO & Founder. All rights reserved.
$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
if (-not (Test-Path (Join-Path $Root "docs\index.html"))) {
    $Root = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
}

Write-Host "ROOT $Root"
Set-Location $Root

Write-Host "`n== docs =="
python (Join-Path $Root "tools\scripts\verify_docs.py")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if (-not (Get-Command gcc -ErrorAction SilentlyContinue)) { Write-Host "FAIL gcc missing"; exit 1 }
if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) { Write-Host "FAIL g++ missing"; exit 1 }

$outDir = Join-Path $Root "build\host"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

$inc = @(
    "-I$Root\platform\common",
    "-I$Root\platform\hal",
    "-I$Root\platform\framework",
    "-I$Root\platform\protocols",
    "-I$Root\platform\services",
    "-I$Root\products",
    "-I$Root\ports\virtual_ecu",
    "-I$Root\ports\arduino",
    "-I$Root\ports\raspberry_pi_5"
)

$src = @(
    "$Root\platform\common\ae_error.c",
    "$Root\platform\framework\ae_ring.c",
    "$Root\platform\hal\hal_host.c",
    "$Root\platform\protocols\can_service.c",
    "$Root\platform\protocols\isotp.c",
    "$Root\platform\protocols\uds.c",
    "$Root\platform\services\dtc.c",
    "$Root\platform\services\ble_auto.c",
    "$Root\platform\services\fault_mgr.c",
    "$Root\platform\services\ota_agent.c",
    "$Root\platform\services\ecu_models.c",
    "$Root\products\product_dids.c",
    "$Root\products\products_connectivity.c",
    "$Root\products\products_diagnostics.c",
    "$Root\products\products_validation.c",
    "$Root\products\products_security_ota.c",
    "$Root\products\products_registry.c",
    "$Root\ports\arduino\aegw_runtime.c"
)

$cflags = @("-std=c11", "-Wall", "-Wextra", "-Werror", "-DAE_HOST=1") + $inc

Write-Host "`n== host unit tests =="
$exe = Join-Path $outDir "test_host.exe"
& gcc @cflags "$Root\platform\common\ae_error.c" "$Root\platform\framework\ae_ring.c" "$Root\tests\unit\test_host.c" -o $exe
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $exe
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "`n== all 17 product use-cases =="
$exe2 = Join-Path $outDir "test_all_products.exe"
& gcc @cflags @src "$Root\tests\unit\test_all_products.c" -o $exe2
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $exe2
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "`n== module coverage tests =="
$exeC = Join-Path $outDir "test_coverage.exe"
& gcc @cflags @src "$Root\tests\unit\test_coverage.c" -o $exeC
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $exeC
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "`n== Arduino HAL host tests =="
$arduinoInc = @("-I$Root\tests\stubs") + $inc
$arduinoObj = Join-Path $outDir "ae_error_arduino.o"
& gcc @cflags -c "$Root\platform\common\ae_error.c" -o $arduinoObj
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
$exeAh = Join-Path $outDir "test_arduino_hal.exe"
& g++ -std=c++17 -Wall -Wextra -Werror -DARDUINO=10813 @arduinoInc `
    "$Root\ports\arduino\AEGW_C3\src\hal_arduino.cpp" `
    "$Root\tests\stubs\arduino_host.cpp" `
    "$Root\tests\unit\test_arduino_hal.cpp" `
    $arduinoObj -o $exeAh
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $exeAh
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "`n== llvm coverage report =="
$covDir = Join-Path $outDir "cov"
New-Item -ItemType Directory -Force -Path $covDir | Out-Null
$covFlags = $cflags + @("-fprofile-instr-generate", "-fcoverage-mapping", "-O0", "-g")
$covExe = Join-Path $covDir "test_coverage.exe"
& clang @covFlags @src "$Root\tests\unit\test_coverage.c" -o $covExe
if ($LASTEXITCODE -eq 0) {
    $env:LLVM_PROFILE_FILE = (Join-Path $covDir "cov.profraw")
    & $covExe | Out-Host
    & llvm-profdata merge -sparse (Join-Path $covDir "cov.profraw") -o (Join-Path $covDir "cov.profdata")
    & llvm-cov report $covExe "-instr-profile=$(Join-Path $covDir 'cov.profdata')" @src
}

Write-Host "`n== compile C objects for C++ link =="
$objs = @()
foreach ($f in $src) {
    $obj = Join-Path $outDir ((Split-Path $f -Leaf) + ".o")
    & gcc @cflags -c $f -o $obj
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    $objs += $obj
}

Write-Host "`n== C++ Virtual ECU / Arduino / Pi 5 =="
$exe3 = Join-Path $outDir "test_cpp_ports.exe"
$cxxflags = @("-std=c++17", "-Wall", "-Wextra", "-Werror", "-DAE_HOST=1") + $inc
& g++ @cxxflags @objs `
    "$Root\ports\virtual_ecu\virtual_ecu.cpp" `
    "$Root\ports\arduino\arduino_port.cpp" `
    "$Root\ports\raspberry_pi_5\rpi5_port.cpp" `
    "$Root\tests\unit\test_cpp_ports.cpp" `
    -o $exe3
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $exe3
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "`n== CMake host tests =="
if (Get-Command cmake -ErrorAction SilentlyContinue) {
    $cmakeDir = Join-Path $Root "build\cmake"
    cmake -S $Root -B $cmakeDir
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    cmake --build $cmakeDir --config Debug
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    ctest --test-dir $cmakeDir -C Debug --output-on-failure
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
} else {
    Write-Host "cmake not on PATH; skipped (GCC host tests above still ran)"
}

Write-Host "`n== refresh Arduino IDE sketch copies =="
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
if ($cli) {
    Write-Host "`n== Arduino CLI compile ESP32-C3 =="
    $sketch = Join-Path $Root "ports\arduino\AEGW_C3"
    & $cli compile --fqbn "esp32:esp32:esp32c3:CDCOnBoot=cdc" $sketch
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Arduino compile failed (core missing?). Host tests still passed."
    }
} else {
    Write-Host "`n== Arduino CLI not on PATH; sketch is ready for Arduino IDE =="
}

exit 0