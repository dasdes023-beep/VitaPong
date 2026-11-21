// Vita Pong - ejemplo nativo con VitaSDK + vita2d
#include <vita2d.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <stdlib.h>
#include <math.h>

#define W 960
#define H 544
#define PAD_W 16
#define PAD_H 100
#define BALL 14
#define PAD_SPEED 7.0f
#define WIN_SCORE 7

typedef struct { float x, y, vx, vy; } Ball;

static float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }

static void reset_ball(Ball *b, int dir) {
    b->x = W / 2 - BALL / 2;
    b->y = H / 2 - BALL / 2;
    b->vx = 6.0f * dir;
    b->vy = ((rand() % 7) - 3) * 1.2f;
}

static int hit(float bx, float by, float px, float py) {
    return bx < px + PAD_W && bx + BALL > px && by < py + PAD_H && by + BALL > py;
}

int main(void) {
    vita2d_init();
    vita2d_set_clear_color(RGBA8(10, 10, 20, 255));
    vita2d_pgf *font = vita2d_load_default_pgf();

    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    SceCtrlData pad;

    float p1y = H / 2 - PAD_H / 2, p2y = p1y;
    int s1 = 0, s2 = 0, over = 0;
    Ball ball;
    reset_ball(&ball, 1);

    int running = 1;
    while (running) {
        sceCtrlPeekBufferPositive(0, &pad, 1);
        if (pad.buttons & SCE_CTRL_START) break;

        if (!over) {
            // Jugador: D-pad o stick izquierdo
            float stick = ((int)pad.ly - 128) / 128.0f;
            if (fabsf(stick) < 0.15f) stick = 0;
            float move = stick * PAD_SPEED;
            if (pad.buttons & SCE_CTRL_UP)   move = -PAD_SPEED;
            if (pad.buttons & SCE_CTRL_DOWN) move =  PAD_SPEED;
            p1y = clampf(p1y + move, 0, H - PAD_H);

            // IA (con un poco de lentitud para que se pueda ganar)
            float target = ball.y + BALL / 2 - PAD_H / 2;
            float diff = target - p2y;
            p2y = clampf(p2y + clampf(diff, -4.5f, 4.5f), 0, H - PAD_H);

            // Bola
            ball.x += ball.vx;
            ball.y += ball.vy;
            if (ball.y < 0)           { ball.y = 0;           ball.vy = -ball.vy; }
            if (ball.y > H - BALL)    { ball.y = H - BALL;    ball.vy = -ball.vy; }

            float p1x = 30, p2x = W - 30 - PAD_W;
            if (ball.vx < 0 && hit(ball.x, ball.y, p1x, p1y)) {
                float rel = (ball.y + BALL / 2 - (p1y + PAD_H / 2)) / (PAD_H / 2);
                ball.vx = fabsf(ball.vx) * 1.06f;
                ball.vy = rel * 6.0f;
                ball.x = p1x + PAD_W;
            }
            if (ball.vx > 0 && hit(ball.x, ball.y, p2x, p2y)) {
                float rel = (ball.y + BALL / 2 - (p2y + PAD_H / 2)) / (PAD_H / 2);
                ball.vx = -fabsf(ball.vx) * 1.06f;
                ball.vy = rel * 6.0f;
                ball.x = p2x - BALL;
            }
            if (ball.vx > 14) ball.vx = 14;
            if (ball.vx < -14) ball.vx = -14;

            if (ball.x < -BALL) { s2++; reset_ball(&ball, 1); }
            if (ball.x > W)     { s1++; reset_ball(&ball, -1); }
            if (s1 >= WIN_SCORE || s2 >= WIN_SCORE) over = 1;
        } else if (pad.buttons & SCE_CTRL_CROSS) {
            s1 = s2 = 0; over = 0; reset_ball(&ball, 1);
        }

        vita2d_start_drawing();
        vita2d_clear_screen();

        for (int y = 0; y < H; y += 30)
            vita2d_draw_rectangle(W / 2 - 2, y, 4, 16, RGBA8(90, 90, 120, 255));

        vita2d_draw_rectangle(30, p1y, PAD_W, PAD_H, RGBA8(255, 255, 255, 255));
        vita2d_draw_rectangle(W - 30 - PAD_W, p2y, PAD_W, PAD_H, RGBA8(255, 255, 255, 255));
        vita2d_draw_rectangle(ball.x, ball.y, BALL, BALL, RGBA8(255, 220, 60, 255));

        vita2d_pgf_draw_textf(font, W / 2 - 90, 60, RGBA8(255, 255, 255, 255), 2.0f, "%d", s1);
        vita2d_pgf_draw_textf(font, W / 2 + 70, 60, RGBA8(255, 255, 255, 255), 2.0f, "%d", s2);
        vita2d_pgf_draw_text(font, 20, H - 15, RGBA8(140, 140, 170, 255), 1.0f,
                             "D-pad / stick izq: mover   START: salir");

        if (over) {
            vita2d_pgf_draw_textf(font, W / 2 - 150, H / 2 - 10, RGBA8(255, 255, 255, 255), 1.8f,
                                  s1 > s2 ? "¡GANASTE!" : "PERDISTE");
            vita2d_pgf_draw_text(font, W / 2 - 130, H / 2 + 40, RGBA8(200, 200, 220, 255), 1.2f,
                                 "Pulsa X para reiniciar");
        }

        vita2d_end_drawing();
        vita2d_swap_buffers();
    }

    vita2d_free_pgf(font);
    vita2d_fini();
    sceKernelExitProcess(0);
    return 0;
}
