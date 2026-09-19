# Post-Mortem — CandyCrushCarloC++ (borrador para PDF en formato APA 7)

> Cómo convertirlo: pegar en Word, fuente **Times New Roman 12**, interlineado **1.5**, márgenes **2.54 cm**, portada + encabezado con título corto y número de página. Mínimo 3 cuartillas sin contar portada ni referencias.

---

## Portada

**SAE Institute México — Licenciatura en Programación de Videojuegos**
**Materia:** Estructuras de Datos · **Proyecto Final:** Match-3 Game Engine
**Título del juego:** CandyCrushCarloC++
**Alumno:** Carlo Igal Shturman Poplawsky · **Matrícula:** 18139
**Motor:** Template SDL3 del docente (WoWPerro/SDL_DrawReady), juego 100% propio encima
**Fecha:** Semana 11

## 1. Introducción

CandyCrushCarloC++ es un Match-3 arcade: tablero de 8×8 con 6 tipos de dulces, derrota por tiempo (90 s) o por movimientos (30), puntaje con multiplicador de cascadas y gema bomba. El flujo de escenas es Menu Principal → Gameplay ⇄ Pausa → Game Over, más una pantalla de Leaderboard con top-5 persistente en `Assets/highscores.txt`. La máquina de estados usa mi `Stack<T>` (Push para entrar, Pop para volver; la Pausa se apila encima del Gameplay sin destruirlo). La detección de matches usa escaneo de líneas rectas y la bomba usa Flood Fill recursivo; confundirlos es el error clásico que el enunciado advierte y que este proyecto demuestra con pruebas (una L de 3 y un cuadro de 2×2 dan Flood 3 y 4 pero Scan 0). Además integré dos estructuras propias no vistas en clase: `ObjectPool<T>` para partículas VFX y `RingBuffer<T>` para el Undo acotado a 3 usos. El arte (12 gemas + fondo) es propio y el juego compila en Release con runtime estático (`/MT`) en carpeta `Build/` portable.

## 2. Análisis de Complejidad

**Escaneo de líneas (detección de matches).** El tablero es n = filas × columnas = 64. El algoritmo recorre cada renglón una vez contando corridas y luego cada columna una vez: cada celda se visita exactamente 2 veces, más un barrido final de marcado. Complejidad temporal **O(n)** y espacial **O(n)** por el arreglo de marcado (64 booleanos en stack, sin heap). En el peor caso (tablero lleno del mismo color) marca las 64 celdas en un solo pase; no hay recursión ni backtracking. La búsqueda de pista (`FindAnyHint`) prueba cada par adyacente (∼112 pares) con una verificación local O(1) amortizada sobre filas/columnas afectadas, es decir **O(n)** con constante mayor, pero solo se ejecuta tras cada turno o tras 5 s de inactividad, nunca por frame, por lo que su costo es despreciable a 60 FPS.

**Flood Fill (bomba).** Recursión de 4 vecinos con matriz de visitados: cada celda se procesa como máximo una vez (los tres casos base —fuera de límites, color distinto, ya visitado— podan el resto), por lo que también es **O(n)** temporal y **O(n)** espacial en visitados, más **O(n)** de pila de llamadas en el peor caso (mancha serpenteante de 64 celdas → 64 marcos de recursión, seguro en stack nativo). ¿Por qué son distintas si ambas son O(n)? Porque responden preguntas distintas: el escaneo pregunta “¿hay k iguales consecutivos en línea recta?” (restringe dirección) y el flood pregunta “¿cuántos conectados hay sin importar la forma?” (permite giros en L/T/cuadros). El peor caso del flood es más costoso en la práctica (recursión + 4 llamadas por celda vs. 2 pasadas iterativas), y como detector de Match-3 es incorrecto: acepta manchas que no son líneas. Por eso el diseño final usa cada uno donde corresponde, con 13 pruebas de lógica que lo blindan (L→Scan 0/Flood 3, 2×2→Scan 0/Flood 4, T→Scan 5 sin duplicar el centro, gravedad, shuffle y snapshots).

## 3. Gestión de Memoria y Arquitectura

**Separación Lógica vs. Renderizado.** `Logic/Board.h` y `Logic/MatchLogic.h` no incluyen nada de SDL (verificado por inspección: solo `MyLib/*` y cabeceras estándar). El tablero guarda enteros (−1 vacío, 0–5 colores) y booleanos de bomba; el `GameplayState` traduce números a sprites (`Assets/gem*.png`), posiciones y animaciones. La aleatoriedad usa un LCG con semilla propia en `Board`, así que la lógica es determinista y testeable sin ventana (el `LogicTest` compila solo `Board.cpp` con `cl` y pasa 13/13).

**Cero leaks.** Toda memoria dinámica tiene un dueño único: `Board` posee sus dos `Grid` (un `new[]` por fila, liberados en el destructor); `GameplayState` posee `Board`, `LinkedQueue`, `RingBuffer` y `ObjectPool` (creados en `Init`, destruidos en `Close`); las texturas e imágenes se liberan en destructores (`Image`, `Text` —ambos tenían leaks en el template: superficie sin destruir, textura reemplazada sin liberar y destructor vacío— corregidos). El punto crítico son las transiciones: el `GameStateManager` nunca borra un estado en medio de su propio `Input/Update` (eso sería use-after-free del `this`); los estados piden cambios (`RequestPush/Pop/Replace/Quit`) y el manager los aplica al final del frame (`ProcessRequests`), haciendo `Close() + delete` en cada `Pop`. Las partículas y snapshots viven en arreglos fijos (una sola reserva por partida), por lo que reiniciar 100 veces no fragmenta el heap. El contador `TNode::contadorVivos` permite auditar nodos vivos en la defensa.

## 4. Justificación de Estructuras Propias

**ObjectPool<T> (VFX).** Cada explosión genera ∼14 partículas y en cascadas hay decenas por segundo. Con `new` por partícula habría cientos de asignaciones/liberaciones por segundo: picos de latencia, fragmentación y riesgo de fuga si una transición interrumpe el ciclo. Con `std::vector + erase` cada muerte sería O(n) por corrimiento y podría reubicar el buffer en pleno frame. El pool reserva un arreglo fijo de 384 `Particle` una vez y recicla índices con una free-list O(1) (`Alloc` = pop, `Free` = push), memoria acotada y 60 FPS estables. Alternativas descartadas: `Trie`/`SparseSet` (indexación por clave, irrelevante aquí) y colas con `new` por evento (el problema que justamente se quiere evitar).

**RingBuffer<T> (Undo).** El reto exige deshacer con límite de 3 usos. Un `Stack` puro de snapshots (8×8×2 + metadatos ≈ 600 B c/u) crecería sin cota en partidas largas; una lista con `new` por turno fragmentaría. El anillo de capacidad 4 (estado base + 3) sobrescribe al más viejo al llenarse: memoria dura acotada (~2.4 KB), `Push/PopRecent` O(1), semántica LIFO dentro de la ventana, perfecta para “los últimos 3 movimientos”. Se eligió sobre `PriorityQueue` con heap (no hay prioridad, solo recencia) y sobre `Quadtree/Octree` (partición espacial, fuera de tema y prohibidas como novedad por ya vistas en clase).

## 5. Conclusión

Construir el juego sobre memoria manual cambia la relación con el código: cada `new` exige decidir quién libera, cuándo y en qué orden, y el manager diferido existe precisamente porque borrar el estado activo a mitad de su método es un error que en Unity/C# el GC escondería. Lo más valioso fue la lección del enunciado —el algoritmo se juzga por la pregunta, no en abstracto— comprobada con pruebas: el mismo Flood Fill que falla como detector es perfecto para la bomba. El resultado es un arcade completo, portable (`Build/` corre con doble clic en otra PC), defendible línea por línea a nivel de RAM.

## Referencias

- Template base: WoWPerro. (2026). *SDL_DrawReady* (SDL3 + SDL_image + SDL_ttf). https://github.com/WoWPerro/SDL_DrawReady.git
- Librería propia: Shturman Poplawsky, C. I. *EstructurasDeDatos26-3* (Stack, Grid, LinkedQueue, Flood Fill).
- Documentación SDL3: https://wiki.libsdl.org/SDL3/FrontPage
