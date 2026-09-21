# CandyCrushCarloC++ 🍬

Juego Match-3 arcade 8x8 con 6 dulces. Hecho en C++ con SDL3.
Usa mi propia libreria: Stack, Grid, Queue + ObjectPool y RingBuffer.
Sin `std::vector`. Codigo simple con nombres completos y comentarios linea por linea.

## Como jugar

- Objetivo: haz puntos antes de que se acaben 90 segundos o 30 movimientos.
- Clic en un dulce y luego en un vecino (arriba, abajo, izquierda, derecha).
- Si forman 3 o mas en linea recta, explotan.
- Bomba: si rompes 4 o mas, nace una bomba. Cambiala con un vecino y rompe toda la mancha de su color.
- Cascada: si al caer se forma otro trio, vale x2, x3.
- Pista: si no juegas 5 segundos, marca 2 dulces en verde.
- Si no hay jugadas, el tablero se mezcla solo.
- Undo: tecla `U` o boton, 3 usos por partida.

## Controles

- Clic: elegir y cambiar dulces.
- `P` o `Esc`: pausa.
- `U`: deshacer.
- `R`: reiniciar.
- `M`: volver al menu.
- Cerrar ventana: salir.

## Escenas

Menu -> Juego -> Pausa -> Game Over. Mas Leaderboard con los 5 mejores (`Assets/highscores.txt`).
La pausa se apila encima del juego sin borrarlo (Push/Pop con mi Stack).

## Estructuras (todas mias)

- `Stack` (`MyLib/Stack.h`): guarda las escenas. Push = ir, Pop = volver.
- `Grid` (`MyLib/Grid.h`): tablero 8x8. Tiene FloodFill para la bomba.
- `LinkedQueue` (`MyLib/LinkedQueue.h`): cola de celdas a romper. Se vacia en orden.
- `ObjectPool` (`MyLib/ObjectPool.h`, propia 1): 384 espacios para chispas. Modo simple: 4 cuadritos fijos por explosion, sin fisica.
- `RingBuffer` (`MyLib/RingBuffer.h`, propia 2): guarda 4 fotos del tablero para 3 undos.
- `TNode` (`MyLib/TNode.h`): eslabon que usan Stack y Queue.

Regla de oro: `MyLib/` y `Logic/` (`Board`, `MatchLogic`) no usan SDL. Solo numeros. El juego lee los numeros y los dibuja.

Detectar trios = escaneo de lineas (horizontal + vertical). FloodFill solo se usa para la bomba.

## Retos (5 hechos, reclamo 4)

1. Highscores en archivo + pantalla Leaderboard.
2. Undo con limite de 3.
3. Cascadas x2, x3 (al instante, sin animacion).
4. Pista a los 5 s (marco fijo, sin parpadeo).
5. Shuffle si no hay jugadas.

Modo simple a proposito: sin caida suave, sin esperas, sin escalas, sin fisica. Todo se resuelve al instante para que sea facil de explicar.

## Compilar y correr

1. Abrir `SDLDrawReady.sln` en Visual Studio 2022, x64.
2. Compilar en Release.
3. Doble clic a `x64\Release\SDLDrawReady.exe`.

## Archivos

- `SDLDrawReady/MyLib/`: mis estructuras.
- `SDLDrawReady/Logic/`: `Board` + `MatchLogic` (logica pura).
- `SDLDrawReady/States/`: Menu, Gameplay, Pause, GameOver, Leaderboard.
- `SDLDrawReady/Assets/`: dulces, bombas, fondo, botones, fuente.
- `SDLDrawReady/ExternalLibs/`: SDL3 listo para compilar.
- `docs/`: post-mortem.

## Creditos

- Base: `WoWPerro/SDL_DrawReady` (SDL3, MIT).
- Arte gratis: "Match-3 Game Asset UI Effects Free" (Cutie Tutti Frutti, Ajay Karat).
- Libreria: `EstructurasDeDatos26-3` (Igal Shturman Poplawsky).

## Datos

Carlo Igal Shturman Poplawsky, matricula 18139, SAE Mexico, Estructuras de Datos.
