#pragma once
// =====================================================================
//  Game/CandyConfig.h - Constantes de diseno (balance del juego).
//  Un solo lugar para tunear: tablero, tiempos, puntajes, retos.
// =====================================================================

namespace CandyConfig
{
    static const int BOARD_ROWS = 8;
    static const int BOARD_COLS = 8;
    static const int GEM_TYPES = 6;

    // Tablero en pantalla (ventana 1920x1080 del Platform).
    static const int CELL = 96;
    static const int BOARD_X = 560;
    static const int BOARD_Y = 150;
    static const int BOARD_W = BOARD_COLS * CELL; // 768
    static const int BOARD_H = BOARD_ROWS * CELL; // 768

    // Arcade: derrota por tiempo O por movimientos (cualquiera agota).
    static const float GAME_TIME = 90.0f; // segundos
    static const int GAME_MOVES = 30;

    // Puntaje: base por gema x nivel de cascada (x1, x2, x3...).
    static const int SCORE_PER_GEM = 10;
    static const int BONUS_BOMB_CREATED = 50;
    static const int BONUS_BOMB_DETONATED_PER_GEM = 15;

    // Undo: 3 usos por partida (RingBuffer capacidad 4 = base + 3).
    static const int UNDO_CAPACITY = 4;
    static const int UNDO_USES = 3;

    // Hint: segundos sin interactuar antes de palpitar la pista.
    static const float HINT_IDLE_TIME = 5.0f;

    // Animaciones (segundos).
    static const float POP_TIME = 0.28f;
    static const float SWAP_TIME = 0.16f;
    static const float FALL_LERP = 10.0f; // velocidad de caida suave

    // Particulas (ObjectPool acotado: 60 FPS garantizados).
    static const int PARTICLE_POOL = 384;

    // Archivos.
    static const char* FILE_HIGHSCORES = "Assets/highscores.txt";
    static const char* FILE_FONT = "Assets/font.ttf";
    static const char* FILE_BG = "Assets/bg.png";
    static const int HIGHSCORE_MAX = 5;
}
