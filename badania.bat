@echo off
setlocal enabledelayedexpansion

REM =======================================================
REM FLAGI STERUJACE (1 = wlacz, 0 = wylacz)
REM =======================================================
set "RUN_PHASE_1=1"
set "RUN_PHASE_2=1"
set "RUN_PHASE_3=1"
set "RUN_PHASE_4=1"

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

REM Nowy, zaktualizowany naglowek CSV
set "CSV_HEADER=Algorytm;Rozmiar;Repetycje;UB_Strategy;Sasiedztwo;TS_MaxIter;TS_Tenure;TS_Asp;Koszt;Optimum;Blad_wzgledny_%%;Czas_us"

set "DEF_TS_MAX=1000"
set "DEF_TS_ASP=1"
set "RAND_ITER=1000"


REM =======================================================
REM FAZA 1: Tabu Search - Basic i wplyw UB/LB (INF vs RNN)
REM =======================================================
if "%RUN_PHASE_1%"=="1" (
    echo =======================================================
    echo FAZA 1: Tabu Search - Basic i wplyw UB/LB
    echo =======================================================

    for %%A in (TABU_SEARCH) do (
        for %%P in (tsp atsp) do (
            for %%U in (INF RNN) do (
                set "OUT=.\results\ts_faza1_basic_%%P_UB_%%U.csv"
                if not exist "!OUT!" (
                    echo %CSV_HEADER%> "!OUT!"
                )

                set "SYM=0"
                if %%P==tsp set "SYM=1"

                for /L %%N in (5,1,12) do (
                    set /a TENURE=%%N/2
                    echo -^> %%A ^| %%P ^| N=%%N ^| UB=%%U ^| TENURE=!TENURE!

                    for /L %%I in (1,1,20) do (
                        set "IDX=0%%I"
                        set "IDX=!IDX:~-2!"
                        set "IN=.\data\%%P\matrix_%%Nx%%N_!IDX!.%%P"
                        if exist "!IN!" (
                            set "ALGO=%%A"
                            set "UB=%%U"
                            set "META=TWO_OPT"
                            set "TS_MAX=%DEF_TS_MAX%"
                            set "TS_TEN=!TENURE!"
                            set "TS_ASP=%DEF_TS_ASP%"
                            set "N_SIZE=%%N"
                            call :RunWithConfig
                        )
                    )
                )
            )
        )
    )
) else (
    echo Pominieto Faze 1.
)


REM =======================================================
REM FAZA 2: Tabu Search - Wplyw wyboru sasiedztwa (SWAP vs 2-OPT)
REM =======================================================
if "%RUN_PHASE_2%"=="1" (
    echo =======================================================
    echo FAZA 2: Tabu Search - Strojenie (Sasiedztwo)
    echo =======================================================

    for %%A in (TABU_SEARCH) do (
        for %%P in (tsp atsp) do (
            for %%M in (SWAP TWO_OPT) do (
                set "OUT=.\results\ts_faza2_sasiedztwo_%%P_%%M.csv"
                if not exist "!OUT!" (
                    echo %CSV_HEADER%> "!OUT!"
                )

                set "SYM=0"
                if %%P==tsp set "SYM=1"

                echo -^> %%A ^| %%P ^| N=12 ^| SASIEDZTWO=%%M

                for /L %%I in (1,1,5) do (
                    set "IDX=0%%I"
                    set "IDX=!IDX:~-2!"
                    set "IN=.\data\%%P\matrix_12x12_!IDX!.%%P"
                    if exist "!IN!" (
                        set "ALGO=%%A"
                        set "UB=RNN"
                        set "META=%%M"
                        set "TS_MAX=%DEF_TS_MAX%"
                        set "TS_TEN=6"
                        set "TS_ASP=%DEF_TS_ASP%"
                        set "N_SIZE=12"
                        call :RunWithConfig
                    )
                )
            )
        )
    )
) else (
    echo Pominieto Faze 2.
)


REM =======================================================
REM FAZA 3: Tabu Search - Wplyw kadencji (Tenure) i Aspiracji
REM =======================================================
if "%RUN_PHASE_3%"=="1" (
    echo =======================================================
    echo FAZA 3: Tabu Search - Strojenie (Kadencja i Aspiracja)
    echo =======================================================

    for %%A in (TABU_SEARCH) do (
        for %%P in (tsp atsp) do (
            REM Testujemy rozne kadencje dla N=12: 6 (N/2), 10 (stala), 12 (N)
            for %%T in (6 10 12) do (
                for %%S in (0 1) do (
                    set "OUT=.\results\ts_faza3_params_%%P_Tenure_%%T_Asp_%%S.csv"
                    if not exist "!OUT!" (
                        echo %CSV_HEADER%> "!OUT!"
                    )

                    set "SYM=0"
                    if %%P==tsp set "SYM=1"

                    echo -^> %%A ^| %%P ^| N=12 ^| TENURE=%%T ^| ASP=%%S

                    for /L %%I in (1,1,5) do (
                        set "IDX=0%%I"
                        set "IDX=!IDX:~-2!"
                        set "IN=.\data\%%P\matrix_12x12_!IDX!.%%P"
                        if exist "!IN!" (
                            set "ALGO=%%A"
                            set "UB=RNN"
                            set "META=TWO_OPT"
                            set "TS_MAX=%DEF_TS_MAX%"
                            set "TS_TEN=%%T"
                            set "TS_ASP=%%S"
                            set "N_SIZE=12"
                            call :RunWithConfig
                        )
                    )
                )
            )
        )
    )
) else (
    echo Pominieto Faze 3.
)


REM =======================================================
REM FAZA 4: TSPLIB - max size challenge (15 min timeout)
REM =======================================================
if "%RUN_PHASE_4%"=="1" (
    echo =======================================================
    echo FAZA 4: TSPLIB - (Weryfikacja maksymalnych instancji)
    echo =======================================================

    for %%A in (TABU_SEARCH) do (
        for %%P in (tsp atsp) do (
            set "OUT=.\results\ts_faza4_tsplib_%%P.csv"
            if not exist "!OUT!" (
                echo %CSV_HEADER%> "!OUT!"
            )

            set "SYM=0"
            if %%P==tsp set "SYM=1"

            for %%F in (.\tsplib\%%P\*.*) do (
                set "IN=%%F"
                echo -^> %%A ^| %%P ^| TSPLIB: %%~nxF

                if exist "!IN!" (
                    set "ALGO=%%A"
                    set "UB=RNN"
                    set "META=TWO_OPT"
                    set "TS_MAX=5000"
                    set "TS_TEN=20"
                    set "TS_ASP=1"
                    set "N_SIZE=0"
                    call :RunWithConfig
                )
            )
        )
    )
) else (
    echo Pominieto Faze 4.
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
    echo repetitions       = 1
    echo randIterations    = %RAND_ITER%
    echo ubStrategy        = !UB!
    echo metaNeighborhood  = !META!
    echo tsMaxIterations   = !TS_MAX!
    echo tsTenure          = !TS_TEN!
    echo tsAspiration      = !TS_ASP!
    echo generateRandom    = 0
    echo symmetric         = !SYM!
    echo inputFile         = !IN!
    echo outputFile        = !OUT!
    echo startInstanceSize = !N_SIZE!
    echo instancesCount    = 1
    echo showProgress      = 0
    echo minWeight         = 1
    echo maxWeight         = 100
) > config.txt

REM Uruchomienie programu glownego z limitem 15 minut
powershell -NoProfile -Command "$p = Start-Process -FilePath '%EXE%' -WindowStyle Hidden -PassThru; $p | Wait-Process -Timeout 915 -ErrorAction SilentlyContinue; if (-not $p.HasExited) { $p | Stop-Process -Force; Write-Host '      [!] Zabito proces: Przekroczono limit 15 minut.' }"

REM Dynamiczny CSV Cleaner (Sprawdza indeks nr 10 = Blad_wzgledny_%)
powershell -NoProfile -Command "$out='!OUT!'; $prev=[int]'!PREV_LINES!'; if (Test-Path $out) { $lines = Get-Content -Path $out; if ($lines.Count -gt $prev) { $last = $lines[-1]; $cols = $last -split ';'; if ($cols.Length -ge 12) { $raw = $cols[10].Trim(); if ($raw -match '^[Bb]rak$') { $lines = $lines[0..($lines.Count-2)]; Set-Content -Path $out -Value $lines; Write-Host '      [!] Odrzucono wynik (Blad > 15%): Brak' } else { $val = [double]($raw -replace ',', '.'); if ($val -gt 15.0) { $lines = $lines[0..($lines.Count-2)]; Set-Content -Path $out -Value $lines; Write-Host \"      [!] Odrzucono wynik (Blad > 15%): $val\" } else { Write-Host \"      [OK] Zapisano wynik (Blad <= 15%): $val\" } } } } }"

exit /b 0