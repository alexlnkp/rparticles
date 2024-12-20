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
typedef struct IntRange IntRange;

enum EmitterType {
    ET_CONSTANT, /* emitter that constantly creates particles at a certain interval       */
    ET_BURST     /* emitter that burst particles upon calling a BurstParticles() function */
};

struct IntRange {
    int lowerBound;
    int upperBound;
};

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
    IntRange burstRange;                        /* number range of particles per burst                       */
    void (*drawFunction)(Particle*);            /* called when particle wants to be rendered                 */
    void (*deathFunction)(Particle*, Emitter*); /* called when particle's age is greater than its lifespan   */
    void (*updateFunction)(Particle*, float);   /* called when particle needs to be updated                  */
};

struct Emitter {
    /*       common fields       */
    enum EmitterType type;       /* emitter type {ET_CONSTANT, ET_BURST} */
    EmitterOptions options;      /* emitter options (see `struct EmitterOptions`) */

    Particle* particles;         /* particle array with size `sizeof(Particle) * maxNumParticles` */
    int numParticles;            /* number of currently living particles */
    int maxNumParticles;         /* max amount of living particles allowed */

    float particleSpawnInterval; /* spawn interval for particles. also used for burst */
    float timeSinceLastSpawn;    /* time since last particle was spawned */
    /* ------------------------- */


    /* only for Burst-type emitters */
    bool isBurstActive;             /* is burst currently active?                   */
    int burstAmount;                /* how many particles will be bursted in total. */
    int particlesSpawned;           /* how many particles were bursted.             */
    /* ---------------------------- */
};

float RandomFloatRange(float min, float max);
int RandomIntRange(int min, int max);
Color GetRandomColorInRange(ColorRange cr);
Vector3 GetRandomVector3InRange(Vector3Range v3r);

void DefaultParticleDraw(Particle* p);
void DefaultParticleOnDeath(Particle* p, Emitter* generator);
void DefaultParticleUpdate(Particle* p, float deltaTime);

Emitter InitParticleEmitter(enum EmitterType type, int maxParticles, float spawnInterval, EmitterOptions pe_opt);

void InitParticle(Particle* p, Vector3Range posRange, Vector3Range velRange, FloatRange lifespanRange, ColorRange colorRange);

void BurstParticles(Emitter* emitter);

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


void DefaultParticleDraw(Particle* p) {
    DrawRectangle(p->pos.x, p->pos.y, 15.0f, 15.0f, p->color);
}


void DefaultParticleOnDeath(Particle* p, Emitter* emitter) {
    /* TODO: finish this */
    (void)(p);
    (void)(emitter);
}


Emitter InitParticleEmitter(enum EmitterType type, int maxParticles, float spawnInterval, EmitterOptions pe_opt) {
    Emitter emitter = {0};

    emitter.type = type;
    emitter.particles = (Particle*)RL_MALLOC(maxParticles * sizeof(Particle));
    emitter.numParticles = 0;
    emitter.maxNumParticles = maxParticles;
    emitter.particleSpawnInterval = spawnInterval;
    emitter.timeSinceLastSpawn = 0.0f;

    emitter.options = pe_opt;
    emitter.options.deathFunction  = (pe_opt.deathFunction)  ?
                                      pe_opt.deathFunction   : DefaultParticleOnDeath;
    emitter.options.drawFunction   = (pe_opt.drawFunction)   ?
                                      pe_opt.drawFunction    : DefaultParticleDraw;
    emitter.options.updateFunction = (pe_opt.updateFunction) ?
                                      pe_opt.updateFunction  : DefaultParticleUpdate;

    return emitter;
}


void InitParticle(Particle* p, Vector3Range posRange, Vector3Range velRange, FloatRange lifespanRange, ColorRange colorRange) {
    p->pos = GetRandomVector3InRange(posRange);
    p->vel = GetRandomVector3InRange(velRange);
    p->lifespan = RandomFloatRange(lifespanRange.lowerBound, lifespanRange.upperBound);
    p->age = 0.0f;

    p->color = GetRandomColorInRange(colorRange);
}


void DefaultParticleUpdate(Particle* p, float deltaTime) {
    p->pos.x += p->vel.x * deltaTime;
    p->pos.y += p->vel.y * deltaTime;
    p->pos.z += p->vel.z * deltaTime;
}


/* shouldn't really be called externally. perhaps it's better to inline? */
void UpdateBurstParticles(Emitter* emitter) {
    if (!emitter->isBurstActive) return;

    while (emitter->numParticles < emitter->maxNumParticles &&
           emitter->particlesSpawned < emitter->burstAmount &&
           emitter->timeSinceLastSpawn >= emitter->particleSpawnInterval) {

        InitParticle(&emitter->particles[emitter->numParticles++],
                      emitter->options.positionRange,
                      emitter->options.velocityRange,
                      emitter->options.lifespanRange,
                      emitter->options.colorRange);

        emitter->particlesSpawned++;
        emitter->timeSinceLastSpawn -= emitter->particleSpawnInterval;
    }

    if (emitter->particlesSpawned >= emitter->burstAmount) {
        emitter->isBurstActive = false;
    }
}


void UpdateParticleEmitter(Emitter* emitter, float deltaTime) {
    emitter->timeSinceLastSpawn += deltaTime;

    for (int i = 0; i < emitter->numParticles; i++) {
        Particle* p = &emitter->particles[i];
        if (p->age < p->lifespan) {
            p->age += deltaTime;
            emitter->options.updateFunction(p, deltaTime);
        } else {
            emitter->options.deathFunction(p, emitter);

            if (i < emitter->numParticles - 1) {
                emitter->particles[i] = emitter->particles[emitter->numParticles - 1];
            }
            emitter->numParticles--;
            i--;
        }
    }

    if (emitter->type == ET_BURST) {
        UpdateBurstParticles(emitter);
        return; /* the rest is not for us */
    }

    /* only for CONSTANT particle emitter */
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


void BurstParticles(Emitter* emitter) {
    if (emitter->type != ET_BURST || emitter->isBurstActive) return;

    emitter->isBurstActive = true;
    emitter->burstAmount = RandomIntRange(emitter->options.burstRange.lowerBound, emitter->options.burstRange.upperBound);
    emitter->particlesSpawned = 0;
    emitter->timeSinceLastSpawn = 0;
}


void KillAllParticles(Emitter* emitter) {
    /* TODO: perhaps we should also stop the burst if it's active when this function is called? */
    for (int i = 0; i < emitter->numParticles; i++) {
        Particle* p = &emitter->particles[i];
        emitter->options.deathFunction(p, emitter);

        if (i < emitter->numParticles - 1) {
            emitter->particles[i] = emitter->particles[emitter->numParticles - 1];
        }
        emitter->numParticles--;
        i--;
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
