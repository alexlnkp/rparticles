#include <time.h>

#include <raylib.h>

#define RPARTICLES_IMPLEMENTATION
#include "rparticles.h"

#include "rlcfg.h"

void drawXparticle(Particle* p) {
    DrawCircle(p->pos.x, p->pos.y, 10.0f, p->color);
}

int main(void) {
    srand(time(NULL));
    InitWindow(TARGET_WINDOW_WIDTH, TARGET_WINDOW_HEIGHT, "particle system");

    SetTargetFPS(TARGET_FPS);
    SetExitKey(KEY_NULL);

    Emitter pe = InitParticleEmitter(MAX_PARTICLES, EMITTER_INTERVAL,
        (Vector3Range) { /* positionRange */
            .lowerBound = { .x = 400, .y = 200, .z = 0 },
            .upperBound = { .x = 680, .y = 520, .z = 0 }
        },
        (Vector3Range) { /* velocityRange */
            .lowerBound = { .x = -100.0f, .y = -100.0f, .z = 0 },
            .upperBound = { .x = 100.0f, .y = 100.0f, .z = 0 }
        },
        (FloatRange) { .lowerBound = 1.5f, .upperBound = 5.0f }, /* lifespanRange */
        (ColorRange) { .lowerBound = RED, .upperBound = BLUE }, /* colorRange */

        drawXparticle, /* drawFunction */
        NULL /* onDeathFunction (WIP) */
    );

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        UpdateParticleEmitter(&pe, deltaTime);

        BeginDrawing(); {
            ClearBackground(BLACK);
            RenderParticles(&pe);

            DrawText(TextFormat("particles: %d", pe.numParticles), 0, 0, 24, WHITE);
        } EndDrawing();
    }

    DestroyParticleEmitter(&pe);
    CloseWindow();

    return 0;
}
