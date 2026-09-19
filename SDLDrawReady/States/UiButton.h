#pragma once
#include "../Platform.h"
#include "../Text.h"

// Boton simple: un rectangulo que se puede clickear.
// Si tiene imagen (fondo) la dibuja estirada, si no pinta un cuadro de color.
// El texto lo crea el estado y me lo pasa ya hecho.
struct UiButton
{
    float x, y, w, h;
    Text* label;   // puede ser nullptr si no hay fuente
    Image* fondo;  // puede ser nullptr (entonces pinto cuadro)

    UiButton() : x(0), y(0), w(0), h(0), label(nullptr), fondo(nullptr) {}
    UiButton(float xx, float yy, float ww, float hh, Text* t, Image* f = nullptr)
        : x(xx), y(yy), w(ww), h(hh), label(t), fondo(f) {}

    // true si el mouse esta adentro del boton
    bool Contains(float mx, float my) const
    {
        return mx >= x && mx <= x + w && my >= y && my <= y + h;
    }

    void Draw(Platform* p, bool hovered) const
    {
        if (p == nullptr) return;
        if (fondo != nullptr && fondo->IsValid())
        {
            // el boton del pack, un poco mas grande si el mouse esta encima
            float extra = hovered ? 6.0f : 0.0f;
            p->RenderImageScaled(fondo, x - extra, y - extra, w + extra * 2, h + extra * 2, 0);
        }
        else
        {
            if (hovered)
                p->FillRect(x, y, w, h, 255, 120, 200, 255);
            else
                p->FillRect(x, y, w, h, 70, 30, 90, 255);
            p->FrameRect(x, y, w, h, 255, 255, 255);
        }
        if (label != nullptr)
            label->Display(x + 24, y + (h - 30) * 0.5f);
    }
};
