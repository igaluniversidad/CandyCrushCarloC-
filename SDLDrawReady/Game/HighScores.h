#pragma once
#include <string>

// =====================================================================
//  Game/HighScores.h - Persistencia de records en TXT (sin SDL).
//  Guarda top-5 en Assets/highscores.txt. Sin std::vector: arreglo fijo.
//  Formato por linea: "<puntos> <nombre>" (nombre sin espacios, max 15).
// =====================================================================

class HighScores
{
public:
    static const int MAX = 5;
    struct Entry { int score; char name[16]; };

private:
    Entry _entries[MAX];
    int _count;

public:
    HighScores();

    void Load(const std::string& path);
    void Save(const std::string& path);
    // Regresa posicion (0-based) si entra al top, -1 si no califica.
    int Qualifies(int score) const;
    void Insert(const std::string& name, int score);
    int Count() const { return _count; }
    const Entry& At(int i) const { return _entries[i]; }
    int Best() const { return _count > 0 ? _entries[0].score : 0; }
};
