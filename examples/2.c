#include <stdio.h>
#include <time.h>

#include <raylib.h>

#define RPARTICLES_IMPLEMENTATION
#include "rparticles.h"

#include "rlcfg.h"

void drawXparticle(Particle* p) {
    DrawSphere(p->pos, 5.0f, p->color);
}

void particleXdied(Particle* p, Emitter* pg) { /* :( */
    (void)(pg);
    TraceLog(LOG_INFO, "Particle died at (%f; %f; %f)", p->pos.x, p->pos.y, p->pos.z);
}

int main(void) {
    srand(time(NULL));
    InitWindow(TARGET_WINDOW_WIDTH, TARGET_WINDOW_HEIGHT, "particle system");

    SetTargetFPS(TARGET_FPS);
    SetExitKey(KEY_NULL);

    EmitterOptions pe_opt = {
        .positionRange = (Vector3Range) {
            .lowerBound = { .x = -400, .y = -100, .z = -200 },
            .upperBound = { .x = 400, .y = 200, .z = 520 }
        },
        .velocityRange = (Vector3Range) {
            .lowerBound = { .x = -100.0f, .y = -100.0f, .z = -100.0f },
            .upperBound = { .x =  100.0f, .y =  100.0f, .z =  100.0f }
        },
        .lifespanRange = (FloatRange) { .lowerBound = 1.5f, .upperBound = 5.0f },
        .colorRange = (ColorRange) { .lowerBound = RED, .upperBound = BLUE },
        .drawFunction = drawXparticle,
        .deathFunction = particleXdied
    };

    Emitter pe = InitParticleEmitter(MAX_PARTICLES, EMITTER_INTERVAL, pe_opt);

    Camera3D cam = {0};
    cam.fovy = 90.0f;
    cam.up = (Vector3){0.0f, 1.0f, 0.0f};
    cam.target = (Vector3){1.0f, 0.0f, 0.0f};

    DisableCursor();

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        UpdateParticleEmitter(&pe, deltaTime);
        UpdateCamera(&cam, CAMERA_FREE);

        BeginDrawing(); {
            ClearBackground(RAYWHITE);
            BeginMode3D(cam); {
                DrawGrid(1000, 1.0f);
                RenderParticles(&pe);

            } EndMode3D();
            DrawText(TextFormat("particles: %d", pe.numParticles), 0, 0, 24, BLACK);
        } EndDrawing();
    }

    EnableCursor();

    DestroyParticleEmitter(&pe);
    CloseWindow();

    return 0;
}
