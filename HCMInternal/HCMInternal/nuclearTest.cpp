#pragma once
#include "nuclearTest.h"
#include <windows.h>

nk_command_buffer* bBuffer = NULL;


void DrawOverlay(nk_command_buffer* b) {
    bBuffer = b;
    if (drawProc)
        drawProc();
    if (bFov) {
        nk_color c = { 0xFF,0xFF,0xFF,0xFF };
        //nk_stroke_line(b, 10, 10, 100, 100, 2, c);
        //nk_stroke_line(b, 110, 110, 200, 200, 1, c);
        int radius = iFov / 2;
        auto r = nk_rect(_drawVp.Width / 2 - radius, _drawVp.Height / 2 - radius, iFov, iFov);
        nk_stroke_circle(b, r, 2, c, 64);
    }

    int radius = iFov / 2;
    auto r = nk_rect(_drawVp.Width / 2 - radius, _drawVp.Height / 2 - radius, iFov, iFov);
    nk_color c = { 0xFF,0xFF,0xFF,0xFF };

    //nk_draw_text(b, r, "TEST", 4, ctx->style.font, c, c);
    bBuffer = NULL;
}