# Vita Pong

## Compilar
Necesitas VitaSDK + vita2d instalados (https://vitasdk.org) y la variable VITASDK definida.

    mkdir build && cd build
    cmake ..
    make

Se genera `VitaPong.vpk`. Cópialo a la Vita (VitaShell, FTP o USB) e instálalo desde VitaShell.

Sin instalar nada local: usa el Docker oficial
    docker run --rm -v "$PWD":/src -w /src vitasdk/vitasdk bash -c "mkdir -p build && cd build && cmake .. && make"

## Controles
D-pad o stick izquierdo: mover. START: salir. X: reiniciar tras terminar. Gana quien llegue a 7.
