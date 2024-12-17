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

typedef struct FloatRange {
    float lowerBound;
    float upperBound;
} FloatRange;

typedef struct Vector3Range {
    Vector3 lowerBound;
    Vector3 upperBound;
} Vector3Range;

typedef struct ColorRange {
    Color lowerBound;
    Color upperBound;
} ColorRange;

typedef struct Particle {
    Vector3 pos;
    Vector3 vel;
    float lifespan;
    float age;
    Color color;
} Particle;

typedef struct ParticleGenerator {
    Vector3Range positionRange;
    Vector3Range velocityRange;
    FloatRange lifespanRange;
    ColorRange colorRange;

    Particle* particles;
    int numParticles;
    int maxNumParticles;

    float particleSpawnInterval;
    float timeSinceLastSpawn;

    void (*drawFunction)(Particle*); /* called when particle wants to be rendered */
    void (*particleOnDeath)(Particle*, struct ParticleGenerator*); /* called when particle's age is greater than its lifespan (WIP) */
} ParticleGenerator;

float RandomFloatRange(float min, float max);
int RandomIntRange(int min, int max);
Color GetRandomColorInRange(ColorRange cr);
Vector3 GetRandomVector3InRange(Vector3Range v3r);
void DefaultDrawParticle(Particle* p);
void DefaultParticleOnDeath(Particle* p, ParticleGenerator* generator);

ParticleGenerator InitParticleGenerator(
    int maxParticles,
    float spawnInterval,
    Vector3Range positionRange,
    Vector3Range velocityRange,
    FloatRange lifespanRange,
    ColorRange colorRange,
    void (*drawFunction)(Particle*),
    void (*particleDeathFunction)(Particle*, ParticleGenerator*));

void InitParticle(
    Particle* p,
    Vector3Range posRange,
    Vector3Range velRange,
    FloatRange lifespanRange,
    ColorRange colorRange);

void UpdateParticleGenerator(ParticleGenerator* generator, float deltaTime);
void RenderParticles(const ParticleGenerator* generator);
void DestroyParticleGenerator(ParticleGenerator* generator);

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


void DefaultParticleOnDeath(Particle* p, ParticleGenerator* generator) {
    /* TODO: finish this */
    (void)(p);
    (void)(generator);
}


ParticleGenerator InitParticleGenerator(
    int maxParticles,
    float spawnInterval,
    Vector3Range positionRange,
    Vector3Range velocityRange,
    FloatRange lifespanRange,
    ColorRange colorRange,
    void (*drawFunction)(Particle*),
    void (*particleDeathFunction)(Particle*, ParticleGenerator*)) {

    ParticleGenerator generator;
    generator.particles = (Particle*)RL_MALLOC(maxParticles * sizeof(Particle));
    generator.numParticles = 0;
    generator.maxNumParticles = maxParticles;
    generator.particleSpawnInterval = spawnInterval;
    generator.timeSinceLastSpawn = 0.0f;
    generator.drawFunction = (drawFunction) ? drawFunction : DefaultDrawParticle;
    generator.particleOnDeath = (particleDeathFunction) ? particleDeathFunction : DefaultParticleOnDeath;

    generator.positionRange = positionRange;
    generator.velocityRange = velocityRange;
    generator.lifespanRange = lifespanRange;
    generator.colorRange = colorRange;

    return generator;
}


void InitParticle(Particle* p, Vector3Range posRange, Vector3Range velRange, FloatRange lifespanRange, ColorRange colorRange) {
    p->pos = GetRandomVector3InRange(posRange);
    p->vel = GetRandomVector3InRange(velRange);
    p->lifespan = RandomFloatRange(lifespanRange.lowerBound, lifespanRange.upperBound);
    p->age = 0.0f;

    p->color = GetRandomColorInRange(colorRange);
}


void UpdateParticleGenerator(ParticleGenerator* generator, float deltaTime) {
    generator->timeSinceLastSpawn += deltaTime;

    for (int i = 0; i < generator->numParticles; i++) {
        Particle* p = &generator->particles[i];
        if (p->age < p->lifespan) {
            p->age += deltaTime;
            p->pos.x += p->vel.x * deltaTime;
            p->pos.y += p->vel.y * deltaTime;
            p->pos.z += p->vel.z * deltaTime;
        } else {
            if (i < generator->numParticles - 1) {
                generator->particles[i] = generator->particles[generator->numParticles - 1];
            }
            generator->numParticles--;
            i--;
        }
    }

    while (generator->timeSinceLastSpawn >= generator->particleSpawnInterval) {
        if (generator->numParticles < generator->maxNumParticles) {
            InitParticle(&generator->particles[generator->numParticles],
                generator->positionRange, generator->velocityRange, generator->lifespanRange, generator->colorRange);
            generator->numParticles++;
        }
        generator->timeSinceLastSpawn -= generator->particleSpawnInterval;
    }
}


void RenderParticles(const ParticleGenerator* generator) {
    for (int i = 0; i < generator->numParticles; i++) {
        Particle* p = &generator->particles[i];
        if (p->age < p->lifespan) {
            generator->drawFunction(p);
        }
    }
}


void DestroyParticleGenerator(ParticleGenerator* generator) {
    free(generator->particles);
    generator->particles = NULL;
    generator->numParticles = 0;
    generator->maxNumParticles = 0;
}

#endif /* RPARTICLES_IMPLEMENTATION */

#endif /* __RPARTICLES_H__ */
