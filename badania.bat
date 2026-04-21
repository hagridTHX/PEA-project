@echo off
setlocal enabledelayedexpansion

set "RUN_PHASE_1=0"
set "RUN_PHASE_2=0"
set "RUN_PHASE_3=1"

REM Sciezka do pliku wykonywalnego
set EXE=.\cmake-build-debug\PEA_project_1.exe

if not exist "%EXE%" (
    echo Blad: Nie znaleziono pliku %EXE%
    echo Upewnij sie, ze skompilowales projekt w CLion.
    pause
    exit /b 1
)

REM === ZABEZPIECZENIE CONFIGU ===
if exist config.txt (
    copy /Y config.txt config_backup.txt >nul
)

if not exist ".\results" mkdir ".\results"

REM =======================================================
REM FAZA 1
REM =======================================================
if "%RUN_PHASE_1%"=="1" (
    echo =======================================================
    echo ROZPOCZYNAM FAZE 1: Porownanie algorytmow B^&B ^(bez UB^)
    echo =======================================================

    for %%A in (BB_BFS BB_DFS BB_BEST) do (
        for %%P in (tsp atsp) do (
            set "OUT=.\results\faza1_%%A_%%P.csv"

            if not exist "!OUT!" (
                echo Algorytm;Rozmiar;Repetycje;Iteracje_RAND;Koszt;Optimum;Blad_wzgledny_%%;Czas_us > "!OUT!"
            )

            REM Okreslenie symetrii na podstawie typu pliku
            set "SYM=0"
            if %%P==tsp set "SYM=1"

            REM Wszystkie algorytmy badamy rowno od N=5 do N=12
            for /L %%N in (5,1,12) do (
                echo -^> Badam: %%A ^| Graf: %%P ^| Rozmiar: N=%%N

                REM Dynamiczne przypisanie liczby iteracji RAND z tabeli dla Projektu 1
                set "RAND_ITER=1000"
                if "%%P"=="tsp" (
                    if "%%N"=="5" set "RAND_ITER=4"
                    if "%%N"=="6" set "RAND_ITER=27"
                    if "%%N"=="7" set "RAND_ITER=36"
                    if "%%N"=="8" set "RAND_ITER=882"
                    if "%%N"=="9" set "RAND_ITER=1008"
                    if "%%N"=="10" set "RAND_ITER=9072"
                    if "%%N"=="11" set "RAND_ITER=181000"
                    if "%%N"=="12" set "RAND_ITER=2990000"
                ) else (
                    if "%%N"=="5" set "RAND_ITER=16"
                    if "%%N"=="6" set "RAND_ITER=54"
                    if "%%N"=="7" set "RAND_ITER=360"
                    if "%%N"=="8" set "RAND_ITER=1764"
                    if "%%N"=="9" set "RAND_ITER=14100"
                    if "%%N"=="10" set "RAND_ITER=90700"
                    if "%%N"=="11" set "RAND_ITER=544000"
                    if "%%N"=="12" set "RAND_ITER=2000000"
                )

                for /L %%I in (1,1,20) do (
                    set "IDX=0%%I"
                    set "IDX=!IDX:~-2!"

                    set "IN=.\data\%%P\matrix_%%Nx%%N_!IDX!.%%P"

                    if exist "!IN!" (
                        (
                            echo algorithm         = %%A
                            echo repetitions       = 1
                            echo randIterations    = !RAND_ITER!
                            echo ubStrategy        = INF
                            echo generateRandom    = 0
                            echo symmetric         = !SYM!
                            echo inputFile         = !IN!
                            echo outputFile        = !OUT!
                            echo startInstanceSize = %%N
                            echo instancesCount    = 1
                            echo showProgress      = 0
                            echo minWeight         = 1
                            echo maxWeight         = 100
                        ) > config.txt

                        "%EXE%" >nul 2>&1
                    )
                )
            )
        )
    )
) ELSE (
    echo Pominiento Faze 1.
)

REM =======================================================
REM FAZA 2
REM =======================================================
if "%RUN_PHASE_2%"=="1" (
    echo =======================================================
    echo ROZPOCZYNAM FAZE 2: Wplyw poczatkowego UB
    echo =======================================================

    for %%A in (BB_BFS BB_DFS BB_BEST) do (
        for %%U in (RAND NN RNN_NO_TIES RNN) do (
            for %%P in (tsp atsp) do (
                set "OUT=.\results\faza2_%%A_UB_%%U_%%P.csv"

                if not exist "!OUT!" (
                    echo Algorytm;Rozmiar;Repetycje;Iteracje_RAND;Koszt;Optimum;Blad_wzgledny_%%;Czas_us > "!OUT!"
                )

                REM Okreslenie symetrii i przypisanie iteracji dla N=12
                set "SYM=0"
                set "RAND_ITER=2000000"
                if %%P==tsp (
                    set "SYM=1"
                    set "RAND_ITER=2990000"
                )

                echo -^> Badam UB: %%A ze startowym UB=%%U ^| Graf: %%P ^| Rozmiar: N=12

                REM Badamy 5 pierwszych instancji dla rozmiaru 12
                for /L %%I in (1,1,5) do (
                    set "IDX=0%%I"
                    set "IDX=!IDX:~-2!"

                    set "IN=.\data\%%P\matrix_12x12_!IDX!.%%P"

                    if exist "!IN!" (
                        (
                            echo algorithm         = %%A
                            echo repetitions       = 1
                            echo randIterations    = !RAND_ITER!
                            echo ubStrategy        = %%U
                            echo generateRandom    = 0
                            echo symmetric         = !SYM!
                            echo inputFile         = !IN!
                            echo outputFile        = !OUT!
                            echo startInstanceSize = 12
                            echo instancesCount    = 1
                            echo showProgress      = 0
                            echo minWeight         = 1
                            echo maxWeight         = 100
                        ) > config.txt

                        "%EXE%" >nul 2>&1
                    )
                )
            )
        )
    )
) ELSE (
    echo Pominiento Faze 2.
)

REM =======================================================
REM FAZA 3
REM =======================================================
if "%RUN_PHASE_3%"=="1" (
    echo =======================================================
    echo ROZPOCZYNAM FAZE 3: TSPLIB (Wszystkie algorytmy)
    echo =======================================================

    REM Przechodzimy przez wszystkie wymagane algorytmy
    for %%A in (BB_BEST) do (
        for %%P in (atsp) do (
            set "OUT=.\results\faza3_%%A_tsplib_%%P.csv"

            if not exist "!OUT!" (
                echo Algorytm;Rozmiar;Repetycje;Iteracje_RAND;Koszt;Optimum;Blad_wzgledny_%%;Czas_us > "!OUT!"
            )

            REM Okreslenie symetrii
            set "SYM=0"
            if %%P==tsp set "SYM=1"

            echo -^> Badam TSPLIB ^| Graf: %%P ^| Algorytm: %%A ^| UB: RNN

            for %%F in (.\tsplib\%%P\*.*) do (
                set "IN=%%F"
                echo      Przetwarzam: %%~nxF

                if exist "!IN!" (
                    (
                        echo algorithm         = %%A
                        echo repetitions       = 1
                        echo randIterations    = 1000
                        echo ubStrategy        = INF
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

                    REM Uruchomienie z limitem 15 minut (900s + zapas na ladowanie)
                    powershell -Command "$p = Start-Process -FilePath '%EXE%' -WindowStyle Hidden -PassThru; $p | Wait-Process -Timeout 915 -ErrorAction SilentlyContinue; if (-not $p.HasExited) { $p | Stop-Process -Force; Write-Host '      [!] Zabito proces: Przekroczono limit 15 minut dla %%~nxF.' }"
                )
            )
        )
    )
) ELSE (
    echo Pominiento Faze 3.
)

REM === PRZYWRACANIE CONFIGU ===
if exist config_backup.txt (
    move /Y config_backup.txt config.txt >nul
)

echo =======================================================
echo BADANIA ZAKONCZONE! Wyniki znajduja sie w folderze .\results
echo =======================================================
pause