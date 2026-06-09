@echo off
setlocal enabledelayedexpansion

REM =======================================================
REM SLOWNIK: PLIKI TSPLIB DO PRZEBADANIA I ICH OPTIMA
REM =======================================================

set "FILES_TO_TEST=burma14.tsp br17.atsp eil51.tsp ftv47.atsp ch130.tsp kro124p.atsp pr1002.tsp pr2392.tsp"

REM --- ZAKRES 1: n < 24 (Limit bledu: 0%) ---
REM TSP: N=14, ATSP: N=17
set "OPT[burma14.tsp]=3323"
set "OPT[br17.atsp]=39"

REM --- ZAKRES 2: 25 < n < 74 (Limit bledu: 50%) ---
REM TSP: N=51, ATSP: N=48
set "OPT[eil51.tsp]=426"
set "OPT[ftv47.atsp]=1776"

REM --- ZAKRES 3: 75 < n < 449 (Limit bledu: 100%) ---
REM TSP: N=130, ATSP: N=100
set "OPT[ch130.tsp]=6110"
set "OPT[kro124p.atsp]=36230"

REM --- ZAKRES 4: 450 < n < 2500 (Limit bledu: 150%) ---
REM UWAGA: TSPLIB nie posiada grafow ATSP w tym rozmiarze! Badamy 2x TSP
REM TSP 1: N=1002, TSP 2: N=2392
set "OPT[pr1002.tsp]=259045"
set "OPT[pr2392.tsp]=378032"


REM =======================================================
REM FLAGI STERUJACE (1 = wlacz, 0 = wylacz)
REM =======================================================
set "RUN_GRID_SEARCH=1"

set "EXE=.\cmake-build-debug\PEA_project_1.exe"

if not exist "%EXE%" (
    echo Blad: Nie znaleziono pliku %EXE%
    echo Upewnij sie, ze skompilowales projekt w CLion.
    pause
    exit /b 1
)

REM Zabezpieczenie obecnego configu
if exist config.txt (
    copy /Y config.txt config_backup.txt >nul
)

if not exist ".\results" mkdir ".\results"
REM === ZAKTUALIZOWANY NAGLOWEK CSV (Idealnie pasujacy do C++) ===
set "CSV_HEADER=Algorytm;Plik;Rozmiar;Repetycje;UB_Strategy;Sasiedztwo;TS_MaxIter;TS_Tenure;TS_Asp;ACO_Ants;ACO_Iter;ACO_Alpha;ACO_Beta;ACO_Rho;Koszt;Optimum;Blad_wzgledny_%%;Czas_us"

REM =======================================================
REM FAZA BADAWCZA: STROJENIE PARAMETROW ACO
REM =======================================================
if "%RUN_GRID_SEARCH%"=="1" (
    echo =======================================================
    echo FAZA 1: STROJENIE PARAMETROW ACO DLA WYBRANYCH PLIKOW TSPLIB
    echo =======================================================

    for %%F in (%FILES_TO_TEST%) do (
        set "FILENAME=%%F"
        set "EXPECTED_OPT=!OPT[%%F]!"

        REM Automatyczne wykrywanie czy to TSP czy ATSP (po rozszerzeniu)
        set "SYM=0"
        echo !FILENAME! | findstr /i "\.tsp$" >nul
        if not errorlevel 1 set "SYM=1"

        set "P_DIR=atsp"
        if "!SYM!"=="1" set "P_DIR=tsp"

        set "IN=.\tsplib\!P_DIR!\!FILENAME!"

        if exist "!IN!" (
            REM Wyciaganie liczby (N) z nazwy pliku
            for /f %%S in ('powershell -NoProfile -Command "if ('!FILENAME!' -match '\d+') { $matches[0] } else { '50' }"') do set "N_REAL=%%S"

            REM =======================================================
            REM USTAWIENIE WYJSCIA: 1 PLIK CSV NA 1 MACIERZ TSPLIB
            REM =======================================================
            set "OUT=.\results\ACO_!FILENAME!.csv"
            if not exist "!OUT!" (
                echo %CSV_HEADER%> "!OUT!"
            )

            REM PETLE Z PARAMETRAMI DO PRZETESTOWANIA
            for %%A in (1.0 2.0) do (
                for %%B in (2.0 4.0) do (
                    for %%R in (0.3 0.6) do (

                        REM --- LOGIKA DOBORU MROWEK I ITERACJI ZALEZNA OD N ---
                        set "ACO_ANTS=!N_REAL!"
                        if !ACO_ANTS! gtr 50 set "ACO_ANTS=50"

                        set "ACO_ITER=100"
                        if !N_REAL! lss 100 set "ACO_ITER=200"

                        echo -^> ACO ^| !FILENAME! (N=!N_REAL!) ^| Alpha=%%A ^| Beta=%%B ^| Rho=%%R ^| Ants=!ACO_ANTS! ^| Iter=!ACO_ITER!
                        set "ALGO=ACO"
                        set "ACO_A=%%A"
                        set "ACO_B=%%B"
                        set "ACO_R=%%R"
                        call :RunWithConfig
                    )
                )
            )
        ) else (
            echo [!] Nie znaleziono pliku: !IN!
        )
    )
)

REM === PRZYWRACANIE CONFIGU ===
if exist config_backup.txt (
    move /Y config_backup.txt config.txt >nul
)

echo =======================================================
echo BADANIA ZAKONCZONE! Wyniki znajduja sie w folderze .\results
echo =======================================================
pause
exit /b 0

REM =======================================================
REM PROCEDURA URUCHOMINIO-FILTRUJACA
REM =======================================================
:RunWithConfig
set "PREV_LINES=0"
for /f %%L in ('powershell -NoProfile -Command "(Get-Content -Path ''!OUT!'' -ErrorAction SilentlyContinue).Count"') do set "PREV_LINES=%%L"

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
    echo startInstanceSize = 0
    echo instancesCount    = 1
    echo showProgress      = 0
    echo minWeight         = 1
    echo maxWeight         = 100
) > config.txt

REM Uruchomienie z bezwzglednym limitem 15 minut
powershell -NoProfile -Command "$p = Start-Process -FilePath '%EXE%' -WindowStyle Hidden -PassThru; $p | Wait-Process -Timeout 915 -ErrorAction SilentlyContinue; if (-not $p.HasExited) { $p | Stop-Process -Force; Write-Host '      [!] Zabito proces: Przekroczono limit 15 minut.' }"

REM === ZAKTUALIZOWANY FILTR POWERSHELL (Indeksy 16 dla kolumny bledu wzglednego) ===
powershell -NoProfile -Command "$out='!OUT!'; $prev=[int]'!PREV_LINES!'; $n_real=[int]'!N_REAL!'; $max_err = 150.0; if ($n_real -lt 24) { $max_err = 0.0 } elseif ($n_real -le 74) { $max_err = 50.0 } elseif ($n_real -le 449) { $max_err = 100.0 }; if (Test-Path $out) { $lines = Get-Content -Path $out; if ($lines.Count -gt $prev) { $new_lines = $lines[$prev..($lines.Count-1)]; $sum = 0.0; $cnt = 0; $brak = $false; foreach ($l in $new_lines) { $cols = $l -split ';'; if ($cols.Length -ge 17) { $raw = $cols[16].Trim(); if ($raw -match '^[Bb]rak$') { $brak = $true } else { $val = [double]($raw -replace ',', '.'); $sum += $val; $cnt++ } } } if ($brak) { $lines = $lines[0..($prev-1)]; Set-Content -Path $out -Value $lines; Write-Host '      [!] Odrzucono wyniki: Brak optimum' } elseif ($cnt -gt 0) { $avg = [math]::Round($sum / $cnt, 2); if ($avg -gt $max_err) { $lines = $lines[0..($prev-1)]; Set-Content -Path $out -Value $lines; Write-Host \"      [!] Odrzucono parametry: Sredni blad $avg% przekracza prog $max_err% dla N=$n_real\"; } else { Write-Host \"      [OK] Sukces! Sredni blad $avg% (Prog: $max_err%)\" } } } }"

exit /b 0