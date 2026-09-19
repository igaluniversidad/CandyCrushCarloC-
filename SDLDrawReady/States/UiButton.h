#pragma once
#include "../Platform.h"
#include "../Text.h"

// =====================================================================
//  States/UiButton.h - Boton rectangular clicable (solo presentacion).
//  Sin allocations por frame: el label Text* lo crea y destruye el estado.
// =====================================================================

struct UiButton
{
    float x, y, w, h;
    Text* label; // puede ser nullptr si no hay fuente

    UiButton() : x(0), y(0), w(0), h(0), label(nullptr) {}
    UiButton(float xx, float yy, float ww, float hh, Text* t)
        : x(xx), y(yy), w(ww), h(hh), label(t) {}

    bool Contains(float mx, float my) const
    {
        return mx >= x && mx <= x + w && my >= y && my <= y + h;
    }

    void Draw(Platform* p, bool hovered) const
    {
        if (p == nullptr) return;
        if (hovered)
            p->FillRect(x, y, w, h, 255, 120, 200, 255);
        else
            p->FillRect(x, y, w, h, 70, 30, 90, 255);
        p->FrameRect(x, y, w, h, 255, 255, 255);
        if (label != nullptr)
            label->Display(x + 18, y + (h - 28) * 0.5f);
    }
};
