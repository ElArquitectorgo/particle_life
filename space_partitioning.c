// Basically Sebastian Lague's implementation on his fluid simulation

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
vector2 pos_to_cell_key(particle* particle, f32 radius);
u32 get_hash(u32 cell_x, u32 cell_y, u32 n);
void update_spatial_lookup(spatial_lookup* slp, particle* particles, f32 radius);

spatial_lookup* create_spatial_lookup(u32 num_particles) {
    spatial_lookup* slp = malloc(sizeof(spatial_lookup));
    slp->pairs = malloc(num_particles * sizeof(spatial_pair));
    slp->start_indices = malloc(num_particles * sizeof(u32));
    slp->count = num_particles;
    return slp;
}

vector2 pos_to_cell_key(particle* particle, f32 radius) {
    return (vector2) {
        .x = (u32) (particle->x / radius),
        .y = (u32) (particle->y / radius)
    };
}

u32 get_hash(u32 cell_x, u32 cell_y, u32 n) {
    return ((u32) cell_x * 15823 + (u32) cell_y * 9737333) % n;
}

int comp(const void* a, const void* b) {
    const spatial_pair* pa = a;
    const spatial_pair* pb = b;
    return (pa->cell_key > pb->cell_key) - (pa->cell_key < pb->cell_key);
}

void update_spatial_lookup(spatial_lookup* slp, particle* particles, f32 radius) {
    for (u32 i = 0; i < slp->count; i++) {
        slp->pairs[i].particle_index = i;
        vector2 cell = pos_to_cell_key(&particles[i], radius);
        slp->pairs[i].cell_key = get_hash(cell.x, cell.y, slp->count);
        slp->start_indices[i] = __UINT32_MAX__;
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

