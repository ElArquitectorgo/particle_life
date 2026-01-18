#include "base.h"
#include "utils.c"
#include "particle.c"

typedef struct {
    u32 cell_key;
    u32 particle_index;
} spatial_pair;


typedef struct {
    spatial_pair* pairs;
    u32* start_indices;
    u32 count;
} spatial_lookup;

spatial_lookup* create_spatial_lookup(u32 num_particles);
u16 pos_to_cell_key(particle* particle, u16 width, u16 height);
void update_spatial_lookup(spatial_lookup* slp, particle* particles);

spatial_lookup* create_spatial_lookup(u32 num_particles) {
    spatial_lookup* slp = malloc(sizeof(spatial_lookup));
    slp->pairs = malloc(num_particles * sizeof(spatial_pair));
    slp->start_indices = malloc(num_particles * sizeof(u32));
    slp->count = num_particles;
    return slp;
}

u32 pos_to_cell_key(particle* particle, u16 width, u16 height) {
    f32 s = (height / 4) / height; // 4x4 grid

    u8 cell_x = (u8) particle->x/ s;
    u8 cell_y = (u8) particle->y / s;

    u32 cell_hash = cell_x * cell_y;

    return cell_hash % 10;
}

int comp(const void* a, const void* b) {
    const spatial_pair* pa = a;
    const spatial_pair* pb = b;
    return (pa->cell_key > pb->cell_key) - (pa->cell_key < pb->cell_key);
}

void update_spatial_lookup(spatial_lookup* slp, particle* particles) {
    for (u32 i = 0; i < slp->count; i++) {
        slp->pairs[i].particle_index = i;
        slp->pairs[i].cell_key = pos_to_cell_key(particles[i], 800, 800);
        slp->start_indices = __UINT32_MAX__;
    }
    qsort(slp->pairs, slp->count, sizeof(spatial_pair), comp);

    for (u32 i = 0; i < slp->count; i++) {
        u32 key = slp->pairs[i].cell_key;
        u32 key_prev = i == 0 ? __UINT32_MAX__ : slp->pairs[i - 1].cell_key;
        if (key != key_prev) {
            slp->start_indices[key] = i;
        }
    }
}

