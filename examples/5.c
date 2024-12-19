#include <math.h>
#include <time.h>

#include <raylib.h>

#define RPARTICLES_IMPLEMENTATION
#include "rparticles.h"

#include "rlcfg.h"

Texture2D penger;

void drawXparticle(Particle* p) {
    // DrawCircle(p->pos.x, p->pos.y, 10.0f, p->color);
    DrawTexture(penger, p->pos.x, p->pos.y, (Color){255, 255, 255, 255});
}

void particleXdied(Particle* p, Emitter* pg) { /* :( */
    (void)(pg);
    (void)(p);
}

int main(void) {
    srand(time(NULL));
    InitWindow(TARGET_WINDOW_WIDTH, TARGET_WINDOW_HEIGHT, "particle system");

    penger = LoadTexture(PENGER_IMAGE_PATH);

    SetTargetFPS(TARGET_FPS);
    SetExitKey(KEY_NULL);

    EmitterOptions pe_opt = {
        .positionRange = (Vector3Range) {
            .lowerBound = { .x = GetRenderWidth()  / 2.0f - penger.width  * 5,
                            .y = GetRenderHeight() / 2.0f - penger.height * 5,
                            .z = 0 },
            .upperBound = { .x = GetRenderWidth()  / 2.0f + penger.width  * 5,
                            .y = GetRenderHeight() / 2.0f + penger.height * 5,
                            .z = 0 }
        },
        .velocityRange = (Vector3Range) {
            .lowerBound = { .x = -1000.0f, .y = -1000.0f, .z = 0 },
            .upperBound = { .x = 1000.0f, .y = 1000.0f, .z = 0 }
        },
        .lifespanRange = (FloatRange) { .lowerBound = 15.0f, .upperBound = 15.0f },
        .colorRange = (ColorRange) { .lowerBound = RED, .upperBound = BLUE },
        .burstRange = (IntRange) { 500, 10000 },
        .drawFunction = drawXparticle,
        .deathFunction = particleXdied,
        .updateFunction = NULL
    };

    Emitter pe = InitParticleEmitter(ET_BURST, 10000, 0.0001f, pe_opt);

    /* big penger pinata */
    bool penger_pinata = true;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        if (penger_pinata) {
            BeginDrawing(); {
                ClearBackground(BLACK);
                DrawTexturePro(penger,
                    (Rectangle){ 0.0f, 0.0f, penger.width, penger.height },
                    (Rectangle){ GetRenderWidth() / 2.0f, GetRenderHeight() / 2.0f, penger.width * 10, penger.height * 10 },
                    (Vector2){ penger.width * 10.0f / 2.0f, penger.height * 10.0f / 2.0f }, 0.0f, (Color){ 255, 255, 255, 255 });
            } EndDrawing();

            if (IsKeyPressed(KEY_SPACE)) {
                penger_pinata = false;
                BurstParticles(&pe);
            }

            continue;
        }

        if (IsKeyPressed(KEY_R)) {
            penger_pinata = true;
            KillAllParticles(&pe);
        }

        UpdateParticleEmitter(&pe, deltaTime);

        BeginDrawing(); {
            ClearBackground(BLACK);
            RenderParticles(&pe);

            DrawText(TextFormat("pengers: %d", pe.numParticles), 0, 0, 24, WHITE);
        } EndDrawing();
    }

    DestroyParticleEmitter(&pe);
    CloseWindow();

    return 0;
}
