#pragma once
// =====================================================================
//  Game/Particle.h - Dato POD para el ObjectPool de VFX.
//  Sin SDL aqui: solo numeros. El Gameplay los integra y dibuja.
// =====================================================================

struct Particle
{
    float x, y;       // posicion en pixeles
    float vx, vy;     // velocidad px/s
    float life;       // tiempo restante
    float maxLife;    // para alpha/fade
    float size;       // lado del cuadrito
    unsigned char r, g, b; // color del dulce que exploto
    bool gravity;     // true = cae, false = chispa libre

    Particle()
        : x(0), y(0), vx(0), vy(0), life(0), maxLife(1),
          size(6), r(255), g(255), b(255), gravity(true) {}
};
