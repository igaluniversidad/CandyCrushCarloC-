#pragma once
#include <string>
#include <cstdio>

// =====================================================================
//  States/FontUtil.h - Resuelve la fuente TTF (sin SDL).
//  Intenta Assets/font.ttf (portable, va en el .zip) y si no existe usa
//  C:\Windows\Fonts\arial.ttf (siempre presente en Windows).
// =====================================================================

inline std::string ResolveFontPath()
{
    FILE* f = nullptr;
    fopen_s(&f, "Assets/font.ttf", "r");
    if (f != nullptr) { fclose(f); return "Assets/font.ttf"; }
    fopen_s(&f, "C:\\Windows\\Fonts\\arial.ttf", "r");
    if (f != nullptr) { fclose(f); return "C:\\Windows\\Fonts\\arial.ttf"; }
    return "Assets/font.ttf";
}
