# Run host verification: docs + all product use-cases + C++ ports.
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
    "$Root\products\products_registry.c"
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
exit $LASTEXITCODE
