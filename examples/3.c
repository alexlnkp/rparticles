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
    TraceLog(LOG_INFO, "PENGER DIED!! at (%f; %f; %f)", p->pos.x, p->pos.y, p->pos.z);
}

void updateXparticle(Particle* p, float deltaTime) {
    static float ctr = 0.0f;

    p->pos.x += sinf(p->vel.x + ctr * deltaTime);
    p->pos.y += cosf(p->vel.y + ctr * deltaTime);

    ctr += 0.01f;
}

int main(void) {
    srand(time(NULL));
    InitWindow(TARGET_WINDOW_WIDTH, TARGET_WINDOW_HEIGHT, "particle system");

    penger = LoadTexture(PENGER_IMAGE_PATH);

    SetTargetFPS(TARGET_FPS);
    SetExitKey(KEY_NULL);

    EmitterOptions pe_opt = {
        .positionRange = (Vector3Range) {
            .lowerBound = { .x = 400, .y = 200, .z = 0 },
            .upperBound = { .x = 680, .y = 520, .z = 0 }
        },
        .velocityRange = (Vector3Range) {
            .lowerBound = { .x = -100.0f, .y = -100.0f, .z = 0 },
            .upperBound = { .x = 100.0f, .y = 100.0f, .z = 0 }
        },
        .lifespanRange = (FloatRange) { .lowerBound = 15.0f, .upperBound = 15.0f },
        .colorRange = (ColorRange) { .lowerBound = RED, .upperBound = BLUE },
        .drawFunction = drawXparticle,
        .deathFunction = particleXdied,
        .updateFunction = updateXparticle
    };

    Emitter pe = InitParticleEmitter(MAX_PARTICLES, EMITTER_INTERVAL, pe_opt);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
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
