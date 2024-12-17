#ifndef   __RPARTICLES_H__
#define   __RPARTICLES_H__

#ifndef RAND_FUNCTION
    #include <stdlib.h>
    #define RAND_FUNCTION rand()
#endif

#ifndef MAX_RAND_VAL
    #define MAX_RAND_VAL RAND_MAX
#endif

/* probably a useless check, but just in case */
#if !defined(Vector3) || !defined(Color)
    #include <raylib.h>
#endif

typedef struct EmitterOptions EmitterOptions;
typedef struct Emitter Emitter;
typedef struct Particle Particle;
typedef struct ColorRange ColorRange;
typedef struct Vector3Range Vector3Range;
typedef struct FloatRange FloatRange;

struct FloatRange {
    float lowerBound;
    float upperBound;
};

struct Vector3Range {
    Vector3 lowerBound;
    Vector3 upperBound;
};

struct ColorRange {
    Color lowerBound;
    Color upperBound;
};

struct Particle {
    Vector3 pos;    /* particle's position    */
    Vector3 vel;    /* particle's velocity    */
    float lifespan; /* particle's lifespan    */
    float age;      /* particle's current age */
    Color color;    /* particle's color       */
};

struct EmitterOptions {
    Vector3Range positionRange;                 /* position range in which particles will spawn randomly     */
    Vector3Range velocityRange;                 /* velocity range which each particle will have              */
    FloatRange lifespanRange;                   /* lifespan range which dictates particle's duration of life */
    ColorRange colorRange;                      /* color range in which a particle may be colored as         */
    void (*drawFunction)(Particle*);            /* called when particle wants to be rendered                 */
    void (*deathFunction)(Particle*, Emitter*); /* called when particle's age is greater than its lifespan   */
};

struct Emitter {
    EmitterOptions options;

    Particle* particles;
    int numParticles;
    int maxNumParticles;

    float particleSpawnInterval;
    float timeSinceLastSpawn;
};

float RandomFloatRange(float min, float max);
int RandomIntRange(int min, int max);
Color GetRandomColorInRange(ColorRange cr);
Vector3 GetRandomVector3InRange(Vector3Range v3r);
void DefaultDrawParticle(Particle* p);
void DefaultParticleOnDeath(Particle* p, Emitter* generator);

Emitter InitParticleEmitter(int maxParticles, float spawnInterval, EmitterOptions pe_opt);

void InitParticle(Particle* p, Vector3Range posRange, Vector3Range velRange, FloatRange lifespanRange, ColorRange colorRange);

void UpdateParticleEmitter(Emitter* emitter, float deltaTime);
void RenderParticles(const Emitter* emitter);
void DestroyParticleEmitter(Emitter* emitter);

#ifdef    RPARTICLES_IMPLEMENTATION

/* maybe <= and >= better? */
#define LOWEST_VAL_INRANGE(x, v) (x.lowerBound.v < x.upperBound.v) ? x.lowerBound.v : x.upperBound.v
#define GREATEST_VAL_INRANGE(x, v) (x.lowerBound.v > x.upperBound.v) ? x.lowerBound.v : x.upperBound.v

#define GET_COLORVAL_IN_RANGE(x, v) RandomIntRange(LOWEST_VAL_INRANGE(x, v), GREATEST_VAL_INRANGE(x, v))
#define GET_VECTORVAL_IN_RANGE(x, v) RandomFloatRange(LOWEST_VAL_INRANGE(x, v), GREATEST_VAL_INRANGE(x, v))


float RandomFloatRange(float min, float max) {
    float scale = RAND_FUNCTION / (float)MAX_RAND_VAL;
    return min + scale * (max - min);
}


int RandomIntRange(int min, int max) {
    return min + RAND_FUNCTION / (RAND_MAX / (max - min + 1) + 1);
}


Color GetRandomColorInRange(ColorRange cr) {
    Color result = {
        .r = GET_COLORVAL_IN_RANGE(cr, r),
        .g = GET_COLORVAL_IN_RANGE(cr, g),
        .b = GET_COLORVAL_IN_RANGE(cr, b),
        .a = GET_COLORVAL_IN_RANGE(cr, a)
    };

    return result;
}


Vector3 GetRandomVector3InRange(Vector3Range v3r) {
    Vector3 result = {
        .x = GET_VECTORVAL_IN_RANGE(v3r, x),
        .y = GET_VECTORVAL_IN_RANGE(v3r, y),
        .z = GET_VECTORVAL_IN_RANGE(v3r, z)
    };

    return result;
}


void DefaultDrawParticle(Particle* p) {
    DrawRectangle(p->pos.x, p->pos.y, 15.0f, 15.0f, p->color);
}


void DefaultParticleOnDeath(Particle* p, Emitter* emitter) {
    /* TODO: finish this */
    (void)(p);
    (void)(emitter);
}


Emitter InitParticleEmitter(int maxParticles, float spawnInterval, EmitterOptions pe_opt) {
    Emitter emitter = {0};
    emitter.particles = (Particle*)RL_MALLOC(maxParticles * sizeof(Particle));
    emitter.numParticles = 0;
    emitter.maxNumParticles = maxParticles;
    emitter.particleSpawnInterval = spawnInterval;
    emitter.timeSinceLastSpawn = 0.0f;
    emitter.options = pe_opt;

    return emitter;
}


void InitParticle(Particle* p, Vector3Range posRange, Vector3Range velRange, FloatRange lifespanRange, ColorRange colorRange) {
    p->pos = GetRandomVector3InRange(posRange);
    p->vel = GetRandomVector3InRange(velRange);
    p->lifespan = RandomFloatRange(lifespanRange.lowerBound, lifespanRange.upperBound);
    p->age = 0.0f;

    p->color = GetRandomColorInRange(colorRange);
}


void UpdateParticleEmitter(Emitter* emitter, float deltaTime) {
    emitter->timeSinceLastSpawn += deltaTime;

    for (int i = 0; i < emitter->numParticles; i++) {
        Particle* p = &emitter->particles[i];
        if (p->age < p->lifespan) {
            p->age += deltaTime;
            p->pos.x += p->vel.x * deltaTime;
            p->pos.y += p->vel.y * deltaTime;
            p->pos.z += p->vel.z * deltaTime;
        } else {
            emitter->options.deathFunction(p, emitter);

            if (i < emitter->numParticles - 1) {
                emitter->particles[i] = emitter->particles[emitter->numParticles - 1];
            }
            emitter->numParticles--;
            i--;
        }
    }

    while (emitter->timeSinceLastSpawn >= emitter->particleSpawnInterval) {
        if (emitter->numParticles < emitter->maxNumParticles) {
            InitParticle(&emitter->particles[emitter->numParticles],
                          emitter->options.positionRange,
                          emitter->options.velocityRange,
                          emitter->options.lifespanRange,
                          emitter->options.colorRange);
            emitter->numParticles++;
        }
        emitter->timeSinceLastSpawn -= emitter->particleSpawnInterval;
    }
}


void RenderParticles(const Emitter* emitter) {
    for (int i = 0; i < emitter->numParticles; i++) {
        Particle* p = &emitter->particles[i];
        if (p->age < p->lifespan) {
            emitter->options.drawFunction(p);
        }
    }
}


void DestroyParticleEmitter(Emitter* emitter) {
    free(emitter->particles);
    emitter->particles = NULL;
    emitter->numParticles = 0;
    emitter->maxNumParticles = 0;
}

#endif /* RPARTICLES_IMPLEMENTATION */

#endif /* __RPARTICLES_H__ */
