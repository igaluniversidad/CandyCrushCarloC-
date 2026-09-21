# CandyCrushCarloC++ 🍬

Match-3 arcade estilo Candy Crush construido con **tu propia STL** (Stack, Grid, Queue, Flood Fill de `EstructurasDeDatos26-3`) sobre el template SDL3 del docente (`WoWPerro/SDL_DrawReady`).

## Cómo jugar

- **Objetivo:** junta puntos antes de que se acaben los **90 segundos** o los **30 movimientos**.
- **Mover:** clic en un dulce y luego clic en un vecino adyacente (arriba/abajo/izquierda/derecha). Si el intercambio forma **3+ en línea recta** (horizontal o vertical), explota.
- **Bomba (gema explosiva):** conecta **4 o más** y nace una bomba de ese color. **Intercámbiala** con cualquier vecino para detonar **toda la mancha conectada** de su color (Flood Fill, sin importar la forma).
- **Cascadas:** si al caer se forma otro match, puntúa **x2, x3…**.
- **Pista:** si pasas **5 s** sin jugar, dos dulces palpitan en verde (movimiento válido garantizado).
- **Anti-bloqueo:** si no hay movimientos posibles, el tablero **se mezcla solo** (sin crear matches directos).
- **Undo:** botón o tecla `U`, hasta **3 usos** por partida (RingBuffer acotado).

### Controles

| Entrada | Acción |
|---|---|
| Clic / Enter | Jugar, seleccionar e intercambiar |
| `P` o `Esc` | Pausa (apila escena) |
| `U` | Deshacer (3 usos) |
| `R` | Reiniciar partida |
| `M` | Volver al menú |
| Cerrar ventana | Salir |

### Escenas (máquina de estados con tu Stack)

`Menu Principal → Gameplay ⇄ Pausa → Game Over`, más `Leaderboard` (top-5 persistente en `Assets/highscores.txt`). Pausa se apila **encima** del Gameplay sin destruirlo; salir/quitar apila y desapila con `Push`/`Pop`.

## Estructuras (todas propias, cero `std::vector/stack/map`)

| Estructura | Archivo | Uso |
|---|---|---|
| `Stack<T>` | `SDLDrawReady/MyLib/Stack.h` | `GameStateManager`: Push/Pop de escenas O(1) |
| `Grid<T>` | `SDLDrawReady/MyLib/Grid.h` | Tablero 8×8 + `FloodFill` recursivo de la bomba |
| `LinkedQueue<T>` | `SDLDrawReady/MyLib/LinkedQueue.h` | Cola de eventos de destrucción → cascadas |
| `ObjectPool<T>` ⭐ propia 1 | `SDLDrawReady/MyLib/ObjectPool.h` | Pool fijo de 384 partículas VFX (cero `new` por frame, 60 FPS) |
| `RingBuffer<T>` ⭐ propia 2 | `SDLDrawReady/MyLib/RingBuffer.h` | Historial acotado de 4 snapshots para Undo (3 usos) |

**Regla de oro:** `Logic/` (`Board`, `MatchLogic`) **no incluye SDL** — solo números. El `Game Loop` lee la lógica y dibuja. Prueba de lógica pura: `LogicTest` (13/13: la L de 3 y el 2×2 dan Flood 3/4 pero Scan 0; la T cuenta 5 sin duplicar el centro).

### Retos implementados (5, se reclaman 4 — MODO SIMPLE sin animaciones)

1. **Highscores persistentes** — `Assets/highscores.txt` + pantalla Leaderboard.
2. **Undo** — `RingBuffer`, límite 3.
3. **Multiplicador de cascadas** — x2, x3… (instantáneo, sin animación).
4. **Hint a los 5 s** — busca un swap válido, resaltado fijo (sin parpadeo).
5. *(extra)* **Shuffle anti-bloqueo** — garantiza jugada posible sin matches directos.

> MODO SIMPLE: se eliminaron a propósito caída suave, delays de swap/pop, escalas y física de partículas para defensa simple. El `ObjectPool` sigue integrado (4 cuadritos fijos por explosión, sin física).

## Compilar y correr

1. Abrir `SDLDrawReady.sln` en **Visual Studio 2022**, plataforma **x64**.
2. `Debug` para desarrollar, **`Release`** para entregar (runtime estático `/MT` ya configurado).
3. El post-build copia solo `SDL3.dll`, `SDL3_image.dll`, `SDL3_ttf.dll`, `libpng16-16.dll` y `Assets/` al `OutDir`.
4. Doble clic a `x64\Release\SDLDrawReady.exe` o a `Build\SDLDrawReady.exe` (carpeta portable con todo incluido).

> Nota: `ExternalLibs/SDL3_ttf` del template solo traía `arm64`; se agregaron los binarios oficiales `x64/x86` (release 3.2.2) para poder enlazar en tu máquina AMD64.

## Estructura del repo

```
CandyCrushCarloC++/
├── SDLDrawReady.sln
├── SDLDrawReady/
│   ├── MyLib/      Stack, Grid, LinkedQueue (tu librería) + ObjectPool, RingBuffer (propias)
│   ├── Logic/      Board.h/.cpp, MatchLogic.h  (CERO SDL)
│   ├── States/     MainMenu, Gameplay, Pause, GameOver, Leaderboard
│   ├── Game/       CandyConfig, Particle, HighScores
│   ├── Assets/     gem0-5.png, gem0-5_bomb.png, logo, mascot, bg, btn, panel, font, highscores
│   ├── Platform, Image, Text, GameState(Mananager)  (engine del docente + fixes)
│   └── ExternalLibs/ SDL3, SDL3_image, SDL3_ttf (x64/x86/arm64)
├── Build/          juego portable (generado, no se versiona)
└── docs/           post-mortem
```

## Créditos

- Engine base: [WoWPerro/SDL_DrawReady](https://github.com/WoWPerro/SDL_DrawReady) (SDL3 + SDL_image + SDL_ttf, MIT).
- Arte: pack gratuito **"Match-3 Game Asset UI Effects Free" (Cutie Tutti Frutti, arte y animación de Ajay Karat)** — personajes, fondo, logo, botones, paneles y fuente SnowDream. Las bombas son el personaje del pack con franjas y brillo agregados en `docs/gen_pack_assets.py`.
- Librería de estructuras: `EstructurasDeDatos26-3` (Igal Shturman Poplawsky).

## Datos del alumno

- Nombre: Carlo Igal Shturman Poplawsky · Matrícula: 18139 · SAE Institute México — Estructuras de Datos
