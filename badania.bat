@echo off
setlocal enabledelayedexpansion

set "RUN_TS_PHASE_1=1"
set "RUN_TS_PHASE_2=1"
set "RUN_SA_PHASE_3=1"
set "RUN_SA_PHASE_4=1"
set "RUN_META_PHASE_5=1"

set "EXE=.\cmake-build-debug\PEA_project_1.exe"

if not exist "%EXE%" (
    echo Blad: Nie znaleziono pliku %EXE%
    echo Upewnij sie, ze skompilowales projekt w CLion.
    pause
    exit /b 1
)

if exist config.txt (
    copy /Y config.txt config_backup.txt >nul
)

if not exist ".\results" mkdir ".\results"

set "CSV_HEADER=Algorytm;Rozmiar;Repetycje;Iteracje_RAND;Koszt;Optimum;Blad_wzgledny_%%;Czas_us"

set "META=TWO_OPT"

set "DEF_TS_MAX=1000"
set "DEF_TS_TEN=10"
set "DEF_TS_ASP=1"

set "DEF_SA_T0=10000.0"
set "DEF_SA_RATE=0.99"
set "DEF_SA_EPOCH=100"
set "DEF_SA_MIN=0.001"
set "DEF_SA_SCHEMA=GEOMETRIC"

set "RAND_ITER=1000"

if "%RUN_TS_PHASE_1%"=="1" (
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
                            set "TS_MAX=1000"
                            set "TS_TEN=!TENURE!"
                            set "TS_ASP=1"
                            set "SA_T0=%DEF_SA_T0%"
                            set "SA_RATE=%DEF_SA_RATE%"
                            set "SA_EPOCH=%DEF_SA_EPOCH%"
                            set "SA_MIN=%DEF_SA_MIN%"
                            set "SA_SCHEMA=%DEF_SA_SCHEMA%"
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

if "%RUN_TS_PHASE_2%"=="1" (
    echo =======================================================
    echo FAZA 2: Tabu Search - strojenie parametrow
    echo =======================================================

    for %%A in (TABU_SEARCH) do (
        for %%P in (tsp atsp) do (
            for %%T in (2 6 12) do (
                for %%S in (0 1) do (
                    set "OUT=.\results\ts_faza2_params_%%P_Tenure_%%T_Asp_%%S.csv"
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
                            set "TS_MAX=1000"
                            set "TS_TEN=%%T"
                            set "TS_ASP=%%S"
                            set "SA_T0=%DEF_SA_T0%"
                            set "SA_RATE=%DEF_SA_RATE%"
                            set "SA_EPOCH=%DEF_SA_EPOCH%"
                            set "SA_MIN=%DEF_SA_MIN%"
                            set "SA_SCHEMA=%DEF_SA_SCHEMA%"
                            set "N_SIZE=12"
                            call :RunWithConfig
                        )
                    )
                )
            )
        )
    )
) else (
    echo Pominieto Faze 2.
)

if "%RUN_SA_PHASE_3%"=="1" (
    echo =======================================================
    echo FAZA 3: Simulated Annealing - Basic i wplyw UB/LB
    echo =======================================================

    for %%A in (SIMULATED_ANNEALING) do (
        for %%P in (tsp atsp) do (
            for %%U in (INF RNN) do (
                set "OUT=.\results\sa_faza1_basic_%%P_UB_%%U.csv"
                if not exist "!OUT!" (
                    echo %CSV_HEADER%> "!OUT!"
                )

                set "SYM=0"
                if %%P==tsp set "SYM=1"

                for /L %%N in (5,1,12) do (
                    echo -^> %%A ^| %%P ^| N=%%N ^| UB=%%U ^| EPOCH=%%N

                    for /L %%I in (1,1,20) do (
                        set "IDX=0%%I"
                        set "IDX=!IDX:~-2!"
                        set "IN=.\data\%%P\matrix_%%Nx%%N_!IDX!.%%P"
                        if exist "!IN!" (
                            set "ALGO=%%A"
                            set "UB=%%U"
                            set "TS_MAX=%DEF_TS_MAX%"
                            set "TS_TEN=%DEF_TS_TEN%"
                            set "TS_ASP=%DEF_TS_ASP%"
                            set "SA_T0=10000.0"
                            set "SA_RATE=0.99"
                            set "SA_EPOCH=%%N"
                            set "SA_MIN=0.001"
                            set "SA_SCHEMA=GEOMETRIC"
                            set "N_SIZE=%%N"
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

if "%RUN_SA_PHASE_4%"=="1" (
    echo =======================================================
    echo FAZA 4: Simulated Annealing - strojenie parametrow
    echo =======================================================

    for %%A in (SIMULATED_ANNEALING) do (
        for %%P in (tsp atsp) do (
            for %%T in (100.0 10000.0) do (
                for %%S in (GEOMETRIC LINEAR) do (
                    for %%E in (10 100) do (
                        set "OUT=.\results\sa_faza2_params_%%P_Temp_%%T_Schema_%%S_Epoch_%%E.csv"
                        if not exist "!OUT!" (
                            echo %CSV_HEADER%> "!OUT!"
                        )

                        set "SYM=0"
                        if %%P==tsp set "SYM=1"

                        set "COOL=0.99"
                        if %%S==LINEAR set "COOL=10.0"

                        echo -^> %%A ^| %%P ^| N=12 ^| T=%%T ^| SCHEMA=%%S ^| EPOCH=%%E ^| RATE=!COOL!

                        for /L %%I in (1,1,5) do (
                            set "IDX=0%%I"
                            set "IDX=!IDX:~-2!"
                            set "IN=.\data\%%P\matrix_12x12_!IDX!.%%P"
                            if exist "!IN!" (
                                set "ALGO=%%A"
                                set "UB=RNN"
                                set "TS_MAX=%DEF_TS_MAX%"
                                set "TS_TEN=%DEF_TS_TEN%"
                                set "TS_ASP=%DEF_TS_ASP%"
                                set "SA_T0=%%T"
                                set "SA_RATE=!COOL!"
                                set "SA_EPOCH=%%E"
                                set "SA_MIN=0.001"
                                set "SA_SCHEMA=%%S"
                                set "N_SIZE=12"
                                call :RunWithConfig
                            )
                        )
                    )
                )
            )
        )
    )
) else (
    echo Pominieto Faze 4.
)

if "%RUN_META_PHASE_5%"=="1" (
    echo =======================================================
    echo FAZA 5: TSPLIB - max size challenge (15 min timeout)
    echo =======================================================

    for %%A in (TABU_SEARCH SIMULATED_ANNEALING) do (
        for %%P in (tsp atsp) do (
            set "OUT=.\results\meta_faza3_tsplib_%%A_%%P.csv"
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
                    if %%A==TABU_SEARCH (
                        set "TS_MAX=5000"
                        set "TS_TEN=20"
                        set "TS_ASP=1"
                        set "SA_T0=%DEF_SA_T0%"
                        set "SA_RATE=%DEF_SA_RATE%"
                        set "SA_EPOCH=%DEF_SA_EPOCH%"
                        set "SA_MIN=%DEF_SA_MIN%"
                        set "SA_SCHEMA=%DEF_SA_SCHEMA%"
                    ) else (
                        set "TS_MAX=%DEF_TS_MAX%"
                        set "TS_TEN=%DEF_TS_TEN%"
                        set "TS_ASP=%DEF_TS_ASP%"
                        set "SA_T0=10000.0"
                        set "SA_RATE=0.99"
                        set "SA_EPOCH=100"
                        set "SA_MIN=0.001"
                        set "SA_SCHEMA=GEOMETRIC"
                    )
                    set "N_SIZE=0"
                    call :RunWithConfig
                )
            )
        )
    )
) else (
    echo Pominieto Faze 5.
)

if exist config_backup.txt (
    move /Y config_backup.txt config.txt >nul
)

echo =======================================================
echo BADANIA ZAKONCZONE! Wyniki znajduja sie w folderze .\results
echo =======================================================
pause
exit /b 0

:RunWithConfig
set "PREV_LINES=0"
for /f %%L in ('powershell -NoProfile -Command "(Get-Content -Path ''!OUT!'' -ErrorAction SilentlyContinue).Count"') do set "PREV_LINES=%%L"

(
    echo algorithm         = !ALGO!
    echo repetitions       = 1
    echo randIterations    = %RAND_ITER%
    echo ubStrategy        = !UB!
    echo metaNeighborhood  = %META%
    echo tsMaxIterations   = !TS_MAX!
    echo tsTenure          = !TS_TEN!
    echo tsAspiration      = !TS_ASP!
    echo saInitialTemp     = !SA_T0!
    echo saCoolingRate     = !SA_RATE!
    echo saEpochLength     = !SA_EPOCH!
    echo saMinTemp         = !SA_MIN!
    echo saCoolingSchema   = !SA_SCHEMA!
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

powershell -NoProfile -Command "$p = Start-Process -FilePath '%EXE%' -WindowStyle Hidden -PassThru; $p | Wait-Process -Timeout 915 -ErrorAction SilentlyContinue; if (-not $p.HasExited) { $p | Stop-Process -Force; Write-Host '      [!] Zabito proces: Przekroczono limit 15 minut.' }"

powershell -NoProfile -Command "$out='!OUT!'; $prev=[int]'!PREV_LINES!'; if (Test-Path $out) { $lines = Get-Content -Path $out; if ($lines.Count -gt $prev) { $last = $lines[-1]; $cols = $last -split ';'; if ($cols.Length -ge 7) { $raw = $cols[6].Trim(); if ($raw -match '^[Bb]rak$') { $lines = $lines[0..($lines.Count-2)]; Set-Content -Path $out -Value $lines; Write-Host '[!] Odrzucono wynik (Blad > 15%): Brak' } else { $val = [double]($raw -replace ',', '.'); if ($val -gt 15.0) { $lines = $lines[0..($lines.Count-2)]; Set-Content -Path $out -Value $lines; Write-Host \"[!] Odrzucono wynik (Blad > 15%): $val\" } else { Write-Host \"[OK] Zapisano wynik (Blad <= 15%): $val\" } } } } }"

exit /b 0
