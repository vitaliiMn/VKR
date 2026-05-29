param([string]$Compiler = "clang++")

Write-Host "Compiling..." -ForegroundColor Cyan
$flags = @("-std=c++23", "-O2", "-Wall")

$programs = @(
    @{ src = "gen.cpp";  exe = "gen.exe"  },
    @{ src = "main.cpp"; exe = "main.exe" },
    @{ src = "slow.cpp"; exe = "slow.exe" }
)

foreach ($p in $programs) {
    Write-Host "  -> Compiling $($p.src)" -ForegroundColor DarkGray
    & $Compiler $p.src $flags -o $p.exe
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Compilation failed for $($p.src)" -ForegroundColor Red
        exit 1
    }
}

$tests = @(
    @{n=100; k=30; desc="M"},
    @{n=200; k=50; desc="L"},
    @{n=500; k=200; desc="X"}

)

$report = @()
$outputCsv = "results.csv"

foreach ($t in $tests) {
    Write-Host "`nTesting: n=$($t.n), k=$($t.k) ($($t.desc))" -ForegroundColor Yellow
    
    # Генерация теста
    "$($t.n) $($t.k)" | .\gen.exe | Out-File -Encoding ascii in.txt

    # Запуск Fast
    $swFast = [System.Diagnostics.Stopwatch]::StartNew()
    $outFast = (Get-Content in.txt | .\main.exe 2>&1) -join "`n"
    $swFast.Stop()

    # Запуск Slow
    $swSlow = [System.Diagnostics.Stopwatch]::StartNew()
    $outSlow = (Get-Content in.txt | .\slow.exe 2>&1) -join "`n"
    $swSlow.Stop()

    # Парсинг вывода (формат: "flow, cost = X Y")
    $pattern = 'flow,\s*cost\s*=\s*(\d+)\s+(\d+)'
    $m1 = [regex]::Match($outFast, $pattern)
    $m2 = [regex]::Match($outSlow, $pattern)

    $flowFast = if($m1.Success){$m1.Groups[1].Value}else{"?"}
    $costFast = if($m1.Success){$m1.Groups[2].Value}else{"?"}
    $flowSlow = if($m2.Success){$m2.Groups[1].Value}else{"?"}
    $costSlow = if($m2.Success){$m2.Groups[2].Value}else{"?"}

    $isCorrect = ($flowFast -ne "?") -and ($flowSlow -ne "?") -and ($flowFast -eq $flowSlow) -and ($costFast -eq $costSlow)

    # Расчёт времени и ускорения
    $msFast = $swFast.Elapsed.TotalMilliseconds
    $msSlow = $swSlow.Elapsed.TotalMilliseconds
    $speedup = if ($msFast -gt 0) { [math]::Round($msSlow / $msFast, 2) } else { "inf" }

    $report += [PSCustomObject]@{
        N         = $t.n
        K         = $t.k
        Desc      = $t.desc
        Fast_ms   = $msFast.ToString("0.00")
        Slow_ms   = $msSlow.ToString("0.00")
        Speedup_x = $speedup
        Correct   = $isCorrect
        Flow      = $flowFast
        Cost      = $costFast
    }
}

# Вывод в консоль
Write-Host "`nResults Table:" -ForegroundColor Green
$report | Format-Table N, K, Desc, Fast_ms, Slow_ms, Speedup_x, Correct, Flow, Cost -AutoSize

# Экспорт в CSV (открывается в Excel)
$report | Export-Csv -Path $outputCsv -NoTypeInformation -Encoding UTF8 -Delimiter ";"
Write-Host "Saved to $outputCsv (open in Excel)" -ForegroundColor Cyan

# Очистка
Remove-Item in.txt -ErrorAction SilentlyContinue
Write-Host "`nTesting completed." -ForegroundColor Cyan