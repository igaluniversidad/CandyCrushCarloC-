#pragma once // incluir una sola vez
// MatchLogic = detector de trios con escaneo de lineas rectas (lo CORRECTO).
// NO usa FloodFill. Flood cuenta manchas de cualquier forma (sirve para bomba).
// Aqui se buscan corridas de 3+ en horizontal y en vertical.
// Opera sobre Grid<int> de colores para no mezclar Board con dibujos.
// CERO SDL. CERO std::vector. Solo numeros.

class GridIntFwd; // aviso de clase (no se usa directo)
// Dos casillas vecinas que al cambiarse forman trio (para pista y anti-bloqueo).
struct SwapHint
{
    int filaOrigen; // fila de la primera casilla
    int columnaOrigen; // columna de la primera casilla
    int filaDestino; // fila de la vecina
    int columnaDestino; // columna de la vecina
    bool valida; // true = si hay pista
};

namespace MatchLogic
{
    unsigned int LcgNext(unsigned int& semillaAleatoria); // avanza el azar (sin rand global)

    // Escanea filas + columnas buscando corridas de 3 o mas.
    // marcasSalida[tamano filas*columnas] se llena con true donde hay match.
    // indice = fila * totalColumnas + columna. La T/L no se cuenta doble.
    // O(filas*columnas): cada casilla se ve 2 veces maximo. Vacio (-1) nunca cuenta.
    template <class GridT>
    int ScanMatches(GridT* rejillaColores, int totalFilas, int totalColumnas, bool* marcasSalida);

    // Prueba un cambio temporal y dice si formaria trio. O(filas*columnas).
    template <class GridT>
    bool WouldMatchAfterSwap(GridT* rejillaColores, int totalFilas, int totalColumnas,
                             int filaOrigen, int columnaOrigen, int filaDestino, int columnaDestino);

    // Busca el primer cambio vecino que forme trio (pista / anti-bloqueo).
    // Solo prueba derecha y abajo por pareja para no repetir.
    // Peor caso O((filas*columnas)^2), en 8x8 son ~4k pasos: nada a 60 FPS
    // porque solo se llama por turno o cada 5 segundos, no por frame.
    template <class GridT>
    bool FindAnyHint(GridT* rejillaColores, int totalFilas, int totalColumnas, SwapHint& pistaSalida);
}

// ---- Codigo template (aqui mismo para no complicar el proyecto) ----

// Escanea todo el tablero y marca donde hay trio. Regresa cuantas marco.
template <class GridT>
int MatchLogic::ScanMatches(GridT* rejillaColores, int totalFilas, int totalColumnas, bool* marcasSalida)
{
    for (int indice = 0; indice < totalFilas * totalColumnas; ++indice) marcasSalida[indice] = false; // limpia todo en false

    // Horizontales: recorre cada fila de izquierda a derecha.
    for (int fila = 0; fila < totalFilas; ++fila) // por cada fila...
    {
        int inicioRacha = 0; // donde empezo la racha del mismo color
        for (int columna = 1; columna <= totalColumnas; ++columna) // recorre columnas + 1 extra para cerrar
        {
            int colorActual = (columna < totalColumnas) ? rejillaColores->Get(fila, columna) : -9999; // color de hoy (-9999 al final para forzar cierre)
            int colorInicio = rejillaColores->Get(fila, inicioRacha); // color con el que empezo la racha
            bool esIgual = (columna < totalColumnas) && (colorActual == colorInicio) && (colorInicio != -1); // mismo color y no vacio?
            if (esIgual) continue; // sigue la racha, no cortes
            int largoRacha = columna - inicioRacha; // cuantas seguidas hubo
            if (colorInicio != -1 && largoRacha >= 3) // trio o mas y no vacio?
            {
                for (int columnaMarcada = inicioRacha; columnaMarcada < columna; ++columnaMarcada) // marca cada una...
                    marcasSalida[fila * totalColumnas + columnaMarcada] = true; // ...en true (si ya estaba, sigue true: no cuenta doble)
            }
            inicioRacha = columna; // la proxima racha empieza aqui
        }
    }
    // Verticales: recorre cada columna de arriba a abajo (igual que arriba).
    for (int columna = 0; columna < totalColumnas; ++columna) // por cada columna...
    {
        int inicioRacha = 0; // donde empezo la racha
        for (int fila = 1; fila <= totalFilas; ++fila) // recorre filas + 1 extra para cerrar
        {
            int colorActual = (fila < totalFilas) ? rejillaColores->Get(fila, columna) : -9999; // color de hoy
            int colorInicio = rejillaColores->Get(inicioRacha, columna); // color de inicio
            bool esIgual = (fila < totalFilas) && (colorActual == colorInicio) && (colorInicio != -1); // mismo y no vacio?
            if (esIgual) continue; // sigue la racha
            int largoRacha = fila - inicioRacha; // cuantas seguidas
            if (colorInicio != -1 && largoRacha >= 3) // trio o mas?
            {
                for (int filaMarcada = inicioRacha; filaMarcada < fila; ++filaMarcada) // marca cada una...
                    marcasSalida[filaMarcada * totalColumnas + columna] = true; // ...en true
            }
            inicioRacha = fila; // proxima racha aqui
        }
    }
    int totalMarcadas = 0; // contador final
    for (int indice = 0; indice < totalFilas * totalColumnas; ++indice) // cuenta las true...
        if (marcasSalida[indice]) ++totalMarcadas; // ...una por una
    return totalMarcadas; // cuantas casillas son match
}

// Prueba dos vecinas: las cambia un momento, revisa si hay trio, y las regresa.
template <class GridT>
bool MatchLogic::WouldMatchAfterSwap(GridT* rejillaColores, int totalFilas, int totalColumnas,
                                     int filaOrigen, int columnaOrigen, int filaDestino, int columnaDestino)
{
    if (filaOrigen < 0 || filaOrigen >= totalFilas || columnaOrigen < 0 || columnaOrigen >= totalColumnas) return false; // origen fuera = no
    if (filaDestino < 0 || filaDestino >= totalFilas || columnaDestino < 0 || columnaDestino >= totalColumnas) return false; // destino fuera = no
    int distanciaFilas = filaOrigen - filaDestino; if (distanciaFilas < 0) distanciaFilas = -distanciaFilas; // distancia vertical sin signo
    int distanciaColumnas = columnaOrigen - columnaDestino; if (distanciaColumnas < 0) distanciaColumnas = -distanciaColumnas; // distancia horizontal sin signo
    if (distanciaFilas + distanciaColumnas != 1) return false; // no vecinas = no

    int colorOrigen = rejillaColores->Get(filaOrigen, columnaOrigen); // guarda color 1
    int colorDestino = rejillaColores->Get(filaDestino, columnaDestino); // guarda color 2
    if (colorOrigen == -1 || colorDestino == -1) return false; // vacia = no

    rejillaColores->Set(filaOrigen, columnaOrigen, colorDestino); // cambio temporal 1
    rejillaColores->Set(filaDestino, columnaDestino, colorOrigen); // cambio temporal 2

    // Revisa rapido solo las 2 filas y 2 columnas tocadas (con salida temprana).
    bool encontroTrio = false; // aun no hay
    // Filas tocadas (origen y destino):
    int filasRevisadas[2] = { filaOrigen, filaDestino }; // solo estas 2 filas
    for (int indice = 0; indice < 2 && !encontroTrio; ++indice) // por cada fila tocada...
    {
        int fila = filasRevisadas[indice]; // fila a revisar
        int rachaActual = 1; // racha empieza en 1
        for (int columna = 1; columna < totalColumnas; ++columna) // recorre la fila...
        {
            int colorHoy = rejillaColores->Get(fila, columna); // color de hoy
            int colorAyer = rejillaColores->Get(fila, columna - 1); // color anterior
            if (colorHoy != -1 && colorHoy == colorAyer) { rachaActual++; if (rachaActual >= 3) { encontroTrio = true; break; } } // suma, si llega a 3 listo
            else rachaActual = 1; // se corto, reinicia en 1
        }
    }
    // Columnas tocadas (origen y destino):
    int columnasRevisadas[2] = { columnaOrigen, columnaDestino }; // solo estas 2 columnas
    for (int indice = 0; indice < 2 && !encontroTrio; ++indice) // por cada columna tocada...
    {
        int columna = columnasRevisadas[indice]; // columna a revisar
        int rachaActual = 1; // racha en 1
        for (int fila = 1; fila < totalFilas; ++fila) // recorre la columna...
        {
            int colorHoy = rejillaColores->Get(fila, columna); // color de hoy
            int colorAyer = rejillaColores->Get(fila - 1, columna); // color anterior
            if (colorHoy != -1 && colorHoy == colorAyer) { rachaActual++; if (rachaActual >= 3) { encontroTrio = true; break; } } // suma, si 3 listo
            else rachaActual = 1; // se corto
        }
    }

    rejillaColores->Set(filaOrigen, columnaOrigen, colorOrigen); // regresa cambio 1
    rejillaColores->Set(filaDestino, columnaDestino, colorDestino); // regresa cambio 2
    return encontroTrio; // true = este cambio SI forma trio
}

// Busca en todo el tablero el primer cambio vecino que forme trio.
template <class GridT>
bool MatchLogic::FindAnyHint(GridT* rejillaColores, int totalFilas, int totalColumnas, SwapHint& pistaSalida)
{
    pistaSalida.valida = false; // empieza sin pista
    for (int fila = 0; fila < totalFilas; ++fila) // por cada fila...
    {
        for (int columna = 0; columna < totalColumnas; ++columna) // por cada columna...
        {
            if (rejillaColores->Get(fila, columna) == -1) continue; // vacia = salta
            // Solo derecha y abajo (cada pareja se prueba una sola vez).
            const int pasoFila[2] = { 0, 1 }; // 0 = derecha, 1 = abajo
            const int pasoColumna[2] = { 1, 0 }; // 1 = derecha, 0 = abajo
            for (int direccion = 0; direccion < 2; ++direccion) // prueba las 2 direcciones...
            {
                int filaVecina = fila + pasoFila[direccion]; // fila del vecino
                int columnaVecina = columna + pasoColumna[direccion]; // columna del vecino
                if (filaVecina < 0 || filaVecina >= totalFilas || columnaVecina < 0 || columnaVecina >= totalColumnas) continue; // fuera = salta
                if (rejillaColores->Get(filaVecina, columnaVecina) == -1) continue; // vecina vacia = salta
                if (WouldMatchAfterSwap(rejillaColores, totalFilas, totalColumnas, fila, columna, filaVecina, columnaVecina)) // este cambio forma trio?
                {
                    pistaSalida.filaOrigen = fila; pistaSalida.columnaOrigen = columna; pistaSalida.filaDestino = filaVecina; pistaSalida.columnaDestino = columnaVecina; // guarda las 2 casillas
                    pistaSalida.valida = true; // si hay pista
                    return true; // regresa la primera que halle
                }
            }
        }
    }
    return false; // no hallo ninguna = bloqueado
}

// Avanza la semilla del azar (formula LCG). Sin rand() global para que sea determinista.
inline unsigned int MatchLogic::LcgNext(unsigned int& semillaAleatoria)
{
    semillaAleatoria = semillaAleatoria * 1664525u + 1013904223u; // mezcla la semilla
    return (semillaAleatoria >> 16) & 0x7FFFu; // regresa 15 bits de azar
}
