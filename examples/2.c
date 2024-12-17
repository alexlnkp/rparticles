#include <time.h>

#include <raylib.h>

#define RPARTICLES_IMPLEMENTATION
#include "rparticles.h"

void drawXparticle(Particle* p) {
    DrawSphere(p->pos, 5.0f, p->color);
}

int main(void) {
    srand(time(NULL));
    InitWindow(1280, 720, "particle system");

    SetTargetFPS(165);
    SetExitKey(KEY_NULL);

    ParticleGenerator pg = InitParticleGenerator(100, 0.02f,
        (Vector3Range) { /* positionRange */
            .lowerBound = { .x = -400, .y = -100, .z = -200 },
            .upperBound = { .x = 400, .y = 200, .z = 520 }
        },
        (Vector3Range) { /* velocityRange */
            .lowerBound = { .x = -100.0f, .y = -100.0f, .z = -100.0f },
            .upperBound = { .x =  100.0f, .y =  100.0f, .z =  100.0f }
        },
        (FloatRange) { .lowerBound = 1.5f, .upperBound = 5.0f }, /* lifespanRange */
        (ColorRange) { .lowerBound = RED, .upperBound = BLUE }, /* colorRange */

        drawXparticle, /* drawFunction */
        NULL /* onDeathFunction (WIP) */
    );

    Camera3D cam = {0};
    cam.fovy = 90.0f;
    cam.up = (Vector3){0.0f, 1.0f, 0.0f};
    cam.target = (Vector3){1.0f, 0.0f, 0.0f};

    DisableCursor();

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        UpdateParticleGenerator(&pg, deltaTime);
        UpdateCamera(&cam, CAMERA_FREE);

        BeginDrawing(); {
            ClearBackground(RAYWHITE);
            BeginMode3D(cam); {
                DrawGrid(1000, 1.0f);
                RenderParticles(&pg);

            } EndMode3D();
            DrawText(TextFormat("particles: %d", pg.numParticles), 0, 0, 24, BLACK);
        } EndDrawing();
    }

    EnableCursor();

    DestroyParticleGenerator(&pg);
    CloseWindow();

    return 0;
}
