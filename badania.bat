@echo off
setlocal enabledelayedexpansion

REM Sciezka do pliku wykonywalnego
set EXE=.\cmake-build-debug\PEA_project_1.exe

if not exist "%EXE%" (
    echo Blad: Nie znaleziono pliku %EXE%
    echo Upewnij sie, ze skompilowales projekt w CLion.
    pause
    exit /b 1
)

REM === ZABEZPIECZENIE CONFIGU ===
REM Tworzymy kopie zapasowa Twojego oryginalnego pliku config.txt
if exist config.txt (
    copy /Y config.txt config_backup.txt >nul
)

if not exist ".\results" mkdir ".\results"

echo =======================================================
echo ROZPOCZYNAM FAZE 1: Porownanie algorytmow B^&B (bez UB)
echo =======================================================

for %%A in (BB_BFS BB_DFS BB_BEST) do (
    for %%P in (tsp atsp) do (
        set "OUT=.\results\faza1_%%A_%%P.csv"

        if not exist "!OUT!" (
            echo Algorytm;Rozmiar;Repetycje;Iteracje_RAND;Koszt;Optimum;Blad_wzgledny_%%;Czas_us > "!OUT!"
        )

        for /L %%N in (5,1,15) do (
            echo -^> Badam: %%A ^| Graf: %%P ^| Rozmiar: N=%%N

            for /L %%I in (1,1,20) do (
                REM Dodawanie wiodacego zera do numeru instancji (1 -> 01, 10 -> 10)
                set "IDX=0%%I"
                set "IDX=!IDX:~-2!"

                REM Dopasowane do Twoich plikow: np. .\data\tsp\matrix_5x5_01.tsp
                set "IN=.\data\%%P\matrix_%%Nx%%N_!IDX!.%%P"

                if exist "!IN!" (
                    (
                        echo algorithm         = %%A
                        echo repetitions       = 1
                        echo randIterations    = 1000
                        echo ubStrategy        = INF
                        echo generateRandom    = 0
                        echo symmetric         = 0
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

echo =======================================================
echo ROZPOCZYNAM FAZE 2: Wplyw poczatkowego UB
echo =======================================================

for %%A in (BB_DFS BB_BEST) do (
    for %%U in (INF RAND NN RNN_NO_TIES RNN_TIES) do (
        for %%P in (tsp atsp) do (
            set "OUT=.\results\faza2_%%A_UB_%%U_%%P.csv"

            if not exist "!OUT!" (
                echo Algorytm;Rozmiar;Repetycje;Iteracje_RAND;Koszt;Optimum;Blad_wzgledny_%%;Czas_us > "!OUT!"
            )

            echo -^> Badam UB: %%A z startowym UB=%%U ^| Graf: %%P ^| Rozmiar: N=13

            REM Badamy 5 pierwszych instancji dla rozmiaru 13
            for /L %%I in (1,1,5) do (
                set "IDX=0%%I"
                set "IDX=!IDX:~-2!"

                set "IN=.\data\%%P\matrix_13x13_!IDX!.%%P"

                if exist "!IN!" (
                    (
                        echo algorithm         = %%A
                        echo repetitions       = 1
                        echo randIterations    = 1000
                        echo ubStrategy        = %%U
                        echo generateRandom    = 0
                        echo symmetric         = 0
                        echo inputFile         = !IN!
                        echo outputFile        = !OUT!
                        echo startInstanceSize = 13
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

REM === PRZYWRACANIE CONFIGU ===
REM Po zakonczeniu badan podmieniamy roboczy config na ten z kopii zapasowej
if exist config_backup.txt (
    move /Y config_backup.txt config.txt >nul
)

echo =======================================================
echo BADANIA ZAKONCZONE! Wyniki znajduja sie w folderze .\results
echo =======================================================
pause