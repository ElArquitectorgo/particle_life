#include "utils.c"

typedef struct {
    f32 x, y, vx, vy;
    u8 color;
} particle;

particle* create_particles(u32 num_particles);

particle* create_particles(u32 num_particles) {
    particle *particles = malloc(num_particles * sizeof(particle));

    for (int i = 0; i < num_particles; i++) {
        u8 color = i < 200 ? 0 : i < 400 ? 1 : 2;
        particles[i] = (particle) {
            .x = get_rand(),
            .y = get_rand(),
            .vx = 0,
            .vy = 0,
            .color = color
        };
    }
    return particles;
}
