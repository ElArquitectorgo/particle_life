#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "base.h"
#include "particle.c"

u32 num_particles = 600;
f32 dt = 0.002;
f32 friction_half_time = 0.040;
f32 friction_factor = 0.4;
f32 r_max = 0.2;
f32 force_factor = 2;

typedef struct {
    SDL_Texture* texture;
} texture;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    u16 width, height;
} sdl_state;

// System
int init(sdl_state* state);
texture* load_textures(SDL_Renderer *renderer);
void unload_textures(texture* textures);
void draw(sdl_state* state, particle* particles, texture* textures);

// Logic
f32 force(f32 r, f32 a);
float get_attraction(particle* p1, particle* p2, matrix* colors);
void check_boundaries(particle* particles);
void update(particle* particles, matrix* colors);

f32 force(f32 r, f32 a) {
    f32 beta = 0.3;
    if (r < beta) {
        return r / beta - 1;
    } else if (beta < r && r < 1) {
        return a * (1 - ABS(2 * r - 1 - beta) / (1 - beta));
    } else {
        return 0;
    }
}

void check_boundaries(particle* particles) {
    /*for (int i = 0; i < num_particles; i++) {
        if (particles[i].x < 0 && particles[i].vx < 0) particles[i].x = 1;
        else if (particles[i].x > 1 && particles[i].vx > 0) particles[i].x = 0;
        
        if (particles[i].y < 0 && particles[i].vy < 0) particles[i].y = 1;
        else if (particles[i].y > 1 && particles[i].vy > 0) particles[i].y = 0;
    }*/
   for (int i = 0; i < num_particles; i++) {
        if (particles[i].x < 0 || particles[i].x > 1) {
            particles[i].x = (u8) particles[i].x;
            particles[i].vx *= -1;
        }
        if (particles[i].y < 0 || particles[i].y > 1) {
            particles[i].y = (u8) particles[i].y;
            particles[i].vy *= -1;
        }
   }
}

void update(particle* particles, matrix* colors) {
    // update velocities
    for (int i = 0; i < num_particles; i++) {
        f32 total_force_x = 0;
        f32 total_force_y = 0;

        for (int j = 0; j < num_particles; j++) {
            if (i == j) {
                continue;
            }
            f32 rx = particles[j].x - particles[i].x;
            f32 ry = particles[j].y - particles[i].y;
            f32 r = hypotf(rx, ry);

            if (r > 0 && r < r_max) {
                f32 f = force(r / r_max, get_attraction(&particles[i], &particles[j], colors));
                total_force_x += rx / r * f;
                total_force_y += ry / r * f;
            }
        }

        total_force_x *= r_max * force_factor;
        total_force_y *= r_max * force_factor;

        particles[i].vx *= friction_factor;
        particles[i].vy *= friction_factor;

        particles[i].vx += total_force_x * dt;
        particles[i].vy += total_force_y * dt;
    }

    // update positions
    for (int i = 0; i < num_particles; i++) {
        particles[i].x += particles[i].vx * dt;
        particles[i].y += particles[i].vy * dt;
    }

    check_boundaries(particles);
}

int main() {
    sdl_state state;
    state.width = 1200;
    state.height = 800;
    int succes = init(&state);
    if (succes != 0) {
        return 1;
    }

    texture* textures = load_textures(state.renderer);
    particle* particles = create_particles(num_particles);
    matrix* color_matrix = mat_create(3, 3);
    
    //          red    green   yellow
    //        _______________________
    // red    |  0.7 |  0.3  | -0.3  |
    // green  | -0.3 |   1   | -0.3  |
    // yellow |  0   |   0   |   1   |
    //       -------------------------
    
    mat_fill(color_matrix, 0.0f);
    mat_assign(color_matrix, 0, 0, .7);
    mat_assign(color_matrix, 0, 1, .3);
    mat_assign(color_matrix, 0, 2, -0.3);
    mat_assign(color_matrix, 1, 0, -0.3);
    mat_assign(color_matrix, 1, 1, 1);
    mat_assign(color_matrix, 1, 2, -0.3);
    mat_assign(color_matrix, 2, 0, .0);
    mat_assign(color_matrix, 2, 1, .0);
    mat_assign(color_matrix, 2, 2, 1);

    friction_factor = pow(0.5, dt / friction_factor);

    int last_frame_time = 0;
    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                quit = true;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                state.width = event.window.data1;
                state.height = event.window.data2;
                break;
            }
        }

        float dt = (SDL_GetTicks() - last_frame_time) / 1000.0;
        last_frame_time = SDL_GetTicks();

        SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 0);
        SDL_RenderClear(state.renderer);

        // RENDER LOOP START
        // Particles positions goes from 0 to 1. The draw method scales depending on window size
        draw(&state, particles, textures);
        update(particles, color_matrix);
        // RENDER LOOP END
        SDL_RenderPresent(state.renderer);
    }

    free(particles);
    mat_clear(color_matrix);
    unload_textures(textures);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
    return 0;
}

texture* load_textures(SDL_Renderer *renderer) {
    texture* textures = malloc(sizeof(texture) * 3);
    textures[0].texture = IMG_LoadTexture(renderer, "red_dot.png"); 
    textures[1].texture  = IMG_LoadTexture(renderer, "green_dot.png");
    textures[2].texture = IMG_LoadTexture(renderer, "yellow_dot.png");
    return textures;
}

void unload_textures(texture* textures) {
    SDL_DestroyTexture(textures[0].texture);
    SDL_DestroyTexture(textures[1].texture);
    SDL_DestroyTexture(textures[2].texture);
    free(textures);
}

f32 get_attraction(particle* p1, particle* p2, matrix* colors) {
    u8 c1 = p1->color;
    u8 c2 = p2->color;
    return mat_get(colors, c1, c2);
    
}

void draw(sdl_state* state, particle* particles, texture* textures) {
    for (int i = 0; i < num_particles; i++) {
        SDL_FRect rect = {particles[i].x * state->width, particles[i].y * state->height, 16, 16};
        u8 texture_index = 0;
        if (particles[i].color == 1) { texture_index = 1;}
        else if (particles[i].color == 2) { texture_index = 2; }
        SDL_RenderTexture(state->renderer, textures[texture_index].texture, NULL, &rect);
    }
}

int init(sdl_state* state) {
    int succes = 0;
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Initialization failed! Reason: %s\n", SDL_GetError());
        succes = 1;
    }

    state->window = SDL_CreateWindow("SDL3", state->width, state->height, SDL_WINDOW_RESIZABLE);
    if (state->window == NULL) {
        SDL_Log("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        succes = 1;
    }

    state->renderer = SDL_CreateRenderer(state->window, NULL);
    if (state->renderer == NULL) {
        SDL_Log("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        succes = 1;
    }
    return succes;
}