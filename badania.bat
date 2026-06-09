@echo off
setlocal enabledelayedexpansion

REM =======================================================
REM SLOWNIK: PLIKI TSPLIB DO PRZEBADANIA I ICH OPTIMA
REM =======================================================

REM Pełna lista plików (ATSP + wybrane TSP do pokrycia zakresow)
set "FILES_TO_TEST=burma14.tsp br17.atsp eil51.tsp ft53.atsp ft70.atsp ftv33.atsp ftv35.atsp ftv38.atsp ftv44.atsp ftv47.atsp ftv55.atsp ftv64.atsp ftv70.atsp p43.atsp ry48p.atsp ch130.tsp ftv170.atsp kro124p.atsp rbg323.atsp rbg358.atsp rbg403.atsp rbg443.atsp pr1002.tsp pr2392.tsp"

REM --- ZAKRES 1: n < 24 (Limit bledu: 0%) ---
set "OPT[burma14.tsp]=3323"
set "OPT[br17.atsp]=39"

REM --- ZAKRES 2: 25 < n < 74 (Limit bledu: 50%) ---
set "OPT[eil51.tsp]=426"
set "OPT[ft53.atsp]=6905"
set "OPT[ft70.atsp]=38673"
set "OPT[ftv33.atsp]=1286"
set "OPT[ftv35.atsp]=1473"
set "OPT[ftv38.atsp]=1530"
set "OPT[ftv44.atsp]=1613"
set "OPT[ftv47.atsp]=1776"
set "OPT[ftv55.atsp]=1608"
set "OPT[ftv64.atsp]=1839"
set "OPT[ftv70.atsp]=1950"
set "OPT[p43.atsp]=5620"
set "OPT[ry48p.atsp]=14422"

REM --- ZAKRES 3: 75 < n < 449 (Limit bledu: 100%) ---
set "OPT[ch130.tsp]=6110"
set "OPT[ftv170.atsp]=2755"
set "OPT[kro124p.atsp]=36230"
set "OPT[rbg323.atsp]=1326"
set "OPT[rbg358.atsp]=1163"
set "OPT[rbg403.atsp]=2465"
set "OPT[rbg443.atsp]=2720"

REM --- ZAKRES 4: 450 < n < 2500 (Limit bledu: 150%) ---
set "OPT[pr1002.tsp]=259045"
set "OPT[pr2392.tsp]=378032"

REM =======================================================
REM FLAGI STERUJACE
REM =======================================================
set "RUN_GRID_SEARCH=1"

set "EXE=.\cmake-build-debug\PEA_project_1.exe"

if not exist "%EXE%" (
    echo Blad: Nie znaleziono pliku %EXE%
    pause
    exit /b 1
)

if exist config.txt copy /Y config.txt config_backup.txt >nul
if not exist ".\results" mkdir ".\results"

set "CSV_HEADER=Algorytm;Plik;Rozmiar;Repetycje;UB_Strategy;Sasiedztwo;TS_MaxIter;TS_Tenure;TS_Asp;ACO_Ants;ACO_Iter;ACO_Alpha;ACO_Beta;ACO_Rho;Koszt;Optimum;Blad_wzgledny_%%;Czas_us"

if "%RUN_GRID_SEARCH%" NEQ "1" goto :skip_grid_search

echo =======================================================
echo FAZA 1: STROJENIE PARAMETROW ACO
echo =======================================================

for %%F in (%FILES_TO_TEST%) do (
    set "FILENAME=%%F"
    set "EXPECTED_OPT=!OPT[%%F]!"

    set "EXT=!FILENAME:~-4!"
    set "SYM=0"
    if /I "!EXT!"==".tsp" set "SYM=1"

    set "P_DIR=atsp"
    if "!SYM!"=="1" set "P_DIR=tsp"

    set "IN=.\tsplib\!P_DIR!\!FILENAME!"

    if not exist "!IN!" (
        echo [X] Nie znaleziono pliku: !IN!
    )
    if exist "!IN!" (
        for /f %%S in ('powershell -NoProfile -Command "if ('!FILENAME!' -match '\d+') { $matches[0] } else { '50' }"') do set "N_REAL=%%S"

        set "OUT=.\results\ACO_!FILENAME!.csv"
        if not exist "!OUT!" (
            echo %CSV_HEADER%> "!OUT!"
        )

        for %%A in (1.0 2.0) do (
            for %%B in (2.0 4.0) do (
                for %%R in (0.3 0.6) do (

                    REM --- LOGIKA ACO (MROWKI I ITERACJE) ---
                    set "ACO_ANTS=!N_REAL!"
                    REM Minimum 30 mrowek dla najmniejszych, max 50 dla gigantow
                    if !ACO_ANTS! lss 30 set "ACO_ANTS=30"
                    if !ACO_ANTS! gtr 50 set "ACO_ANTS=50"

                    set "ACO_ITER=100"
                    if !N_REAL! lss 100 set "ACO_ITER=400"
                    REM Gigantyczna liczba iteracji by wymusic 0% bledu na malych grafach
                    if !N_REAL! lss 25 set "ACO_ITER=2000"

                    echo -^> ACO ^| !FILENAME! ^(N=!N_REAL!^) ^| A=%%A ^| B=%%B ^| R=%%R ^| Ants=!ACO_ANTS! ^| Iter=!ACO_ITER!
                    set "ALGO=ACO"
                    set "ACO_A=%%A"
                    set "ACO_B=%%B"
                    set "ACO_R=%%R"
                    call :RunWithConfig
                )
            )
        )
    )
)

:skip_grid_search

if exist config_backup.txt move /Y config_backup.txt config.txt >nul
echo =======================================================
echo BADANIA ZAKONCZONE! Wyniki w folderze .\results
echo =======================================================
pause
exit /b 0

:RunWithConfig
set "PREV_LINES=0"
if exist "!OUT!" (
    for /f %%C in ('find /c /v "" ^< "!OUT!"') do set "PREV_LINES=%%C"
)

(
    echo algorithm         = !ALGO!
    echo repetitions       = 10
    echo randIterations    = 1000
    echo ubStrategy        = INF
    echo metaNeighborhood  = TWO_OPT
    echo acoAnts           = !ACO_ANTS!
    echo acoIterations     = !ACO_ITER!
    echo acoAlpha          = !ACO_A!
    echo acoBeta           = !ACO_B!
    echo acoRho            = !ACO_R!
    echo generateRandom    = 0
    echo symmetric         = !SYM!
    echo inputFile         = !IN!
    echo outputFile        = !OUT!
    echo knownOptimum      = !EXPECTED_OPT!
    echo startInstanceSize = 0
    echo instancesCount    = 1
    echo showProgress      = 0
    echo minWeight         = 1
    echo maxWeight         = 100
) > config.txt

powershell -NoProfile -Command "$p = Start-Process -FilePath '%EXE%' -WindowStyle Hidden -PassThru; $p | Wait-Process -Timeout 915 -ErrorAction SilentlyContinue; if (-not $p.HasExited) { $p | Stop-Process -Force; Write-Host '      [X] Zabito proces: Przekroczono limit 15 minut.' }"

powershell -NoProfile -Command "$out='!OUT!'; $prev=[int]'!PREV_LINES!'; $n_real=[int]'!N_REAL!'; $max_err = 150.0; if ($n_real -lt 24) { $max_err = 0.0 } elseif ($n_real -le 74) { $max_err = 50.0 } elseif ($n_real -le 449) { $max_err = 100.0 }; if (Test-Path $out) { $lines = @(Get-Content -Path $out); if ($lines.Count -gt $prev) { $new_lines = $lines[$prev..($lines.Count-1)]; $sum = 0.0; $cnt = 0; $brak = $false; foreach ($l in $new_lines) { $cols = $l -split ';'; if ($cols.Length -ge 17) { $raw = $cols[16].Trim(); if ($raw -match '^[Bb]rak$') { $brak = $true } else { $val = [double]($raw -replace ',', '.'); $sum += $val; $cnt++ } } } if ($brak) { $lines = $lines[0..($prev-1)]; Set-Content -Path $out -Value $lines; Write-Host '      [X] Odrzucono wyniki: Brak optimum' } elseif ($cnt -gt 0) { $avg = [math]::Round($sum / $cnt, 2); if ($avg -gt $max_err) { $lines = $lines[0..($prev-1)]; Set-Content -Path $out -Value $lines; Write-Host \"      [X] Odrzucono: Sredni blad $avg%% przekracza prog $max_err%% (N=$n_real)\"; } else { Write-Host \"      [OK] Sukces! Sredni blad $avg%% (Prog: $max_err%%)\" } } } }"

exit /b 0