#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_main.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>
#include "dmath.h"
#include "queue.h"
#include "main.h"
#include "create_map.h"

#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 216

struct wall {
    v2 a, b;
    int portal;
    int bsector;
    int bwall;
    uint32_t color;
};

struct sector {
    int id;
    size_t firstwall, nwalls;
    float zfloor, zceil;
};

struct {
    SDL_Window* window;
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    uint32_t* pixels;
    bool quit;
    uint8_t colormode;

    node_t* queue_head;

    struct sector_group { struct sector arr[32]; size_t n; } sectors;
    struct wall_group { struct wall arr[128];  size_t n; } walls;

    struct {
        v2 pos;
        float angle;
        int sector;
    } camera;
} state;

static void verline(int x, int y0, int y1, uint32_t color) {
   for (int y = y0; y <= y1; y++) {
        state.pixels[(y * SCREEN_WIDTH) + x] = color;
    }
}

static uint8_t point_in_sector(v2 point1, v2 point2, struct sector sec) {
    int i = 0,
        hits = 0;
    for (i = sec.firstwall; i < sec.firstwall + sec.nwalls; i++)
        if (compare_v2(intersect_segs(state.walls.arr[i].a, state.walls.arr[i].b, point1, point2))) hits++;
    return (hits % 2 == 0) ? false : true;
}

static void render_sector(uint8_t sector, v2 xa, v2 xb, v2 ya, v2 yb, uint8_t via) {
    for (int j = 0; j < state.sectors.arr[sector].nwalls; j++) {                                               // Iterate through sector walls
        if (j + state.sectors.arr[sector].firstwall == via) continue;                                          //     If the wall is being rending by means of this portal, skip
        struct wall* cWall = &state.walls.arr[j + state.sectors.arr[sector].firstwall];                        //     Get the wall to be rendered

        float anglea = normalize_a(angle_between_points(state.camera.pos, cWall->a) + state.camera.angle);     //     The angle between the camera and point A of the wall
        float angleb = normalize_a(angle_between_points(state.camera.pos, cWall->b) + state.camera.angle);     //     The angle between the camera and point B of the wall
        if ((anglea > PI_2 && angleb > PI_2) || (anglea < 0 && angleb < 0)) continue;                          //     If both angles are outside of the fov if the *same direction*, skip


        // Cast rays out from the camera at a 90 degree angle and get the intersection between those rays and the wall
        float l = 25;                                                                                             // How long the rays should b e
        float cos_x = l * sinf(PI_2 - state.camera.angle + PI) + state.camera.pos.x;                            // math
        float cos_y = l * sinf(PI_2 - state.camera.angle + PI) + state.camera.pos.y;                            // math
        float sin_x = l * sinf(-state.camera.angle + PI) + state.camera.pos.x;                                  // math
        float sin_y = l * sinf(state.camera.angle + PI) + state.camera.pos.y;                                   // math
        v2 intersection_h = intersect_segs((v2) { cos_x, sin_y }, (v2) { cos_x, sin_y
        }, cWall->a, cWall->b);   // Intersection between the wall and the horz line
        v2 intersection_v = intersect_segs((v2) { sin_x, cos_y }, (v2) { sin_x, cos_y }, cWall->a, cWall->b);   // Intersection between the wall and the vert line
        //v2 intersection_h = intersect_segs((v2) { cos_x, sin_y }, (v2) { state.camera.pos.x, state.camera.pos.y }, cWall->a, cWall->b);   // Intersection between the wall and the horz line
        //v2 intersection_v = intersect_segs((v2) { sin_x, cos_y }, (v2) { state.camera.pos.x, state.camera.pos.y }, cWall->a, cWall->b);   // Intersection between the wall and the vert line

        // Get the grid points where the wall should be rendered between
        // If wall is contained entirely on screen draw between endpoints
        // If a point is offscreen, render between the intersection points instead
        // TODO: FIX CODE; Currently does not work and has been disabled; just renders between endpoints
        v2 pointa, pointb;
        pointb = cWall->b;
        pointa = cWall->a;

        if (compare_v2(intersection_h) && compare_v2(intersection_v)) {
            //printf("zzzz");
            pointa = intersection_v;
            pointb = intersection_h;
        }
        else if (compare_v2(intersection_h)) {
            pointb = intersection_h;
            if (anglea > PI_2 || anglea < 0) pointa = cWall->b;
            else pointa = cWall->a;
        }
        else if (compare_v2(intersection_v)) {
            pointb = intersection_v;
            if (anglea > PI_2 || anglea < 0) pointa = cWall->b;
            else pointa = cWall->a;
        }
        else {
            pointb = cWall->b;
            pointa = cWall->a;
        }
        pointb = cWall->b;
        pointa = cWall->a;

        // Render the wall
        int steps = 1024;                                                      // How many times to iterate over rendering the wall      
        v2 sx = { -1, -1 }, sy = { -1, -1 }, fx = { -1, -1 }, fy = { -1, -1 }; // The screen coords that the wall ends up rendered over
        bool first = true;                                                     // False if wall gets rendered
        for (int k = 0; k < steps; k++) {
            v2 point = (v2){ pointb.x + (pointa.x - pointb.x) * k / steps, pointb.y + (pointa.y - pointb.y) * k / steps };

            float angle = normalize_a(angle_between_points( state.camera.pos, point ) + state.camera.angle);
            if (angle > PI_2 || angle < 0) continue;

            float distance = seglength(point, state.camera.pos);
            int h = (int)(SCREEN_HEIGHT / distance);
            int y0 = max((SCREEN_HEIGHT / 2) - (h / 2), 0);
            int y1 = min((SCREEN_HEIGHT / 2) + (h / 2), SCREEN_HEIGHT - 1);

            float x0 = SCREEN_WIDTH * angle / PI_2;

            v2 i0 = intersect_segs((v2) { x0, 0 }, (v2) { x0, SCREEN_HEIGHT }, xa, xb);
            v2 i1 = intersect_segs((v2) { x0, 0 }, (v2) { x0, SCREEN_HEIGHT }, ya, yb);
            v2 i2 = intersect_segs((v2) { x0, 0 }, (v2) { x0, SCREEN_HEIGHT }, xa, ya);
            v2 i3 = intersect_segs((v2) { x0, 0 }, (v2) { x0, SCREEN_HEIGHT }, yb, yb);
            if (!compare_v2(i0) && !compare_v2(i1) && !compare_v2(i2) && !compare_v2(i3)) continue;

            if (first) {
                sx = (v2){ x0, y0 };
                sy = (v2){ x0, y1 };
                first = false;
            }
            fx = (v2){ x0, y0 };
            fy = (v2){ x0, y1 };

            if (cWall->portal != 1) {
                verline(x0, 0, y0, 0xFF202020);
                verline(x0, y0, y1, cWall->color);
                verline(x0, y1, SCREEN_HEIGHT - 1, 0xFF505050);
            }
        }

        if (cWall->portal == 1 && !first) {
            enqueue(&state.queue_head, (node_t) { cWall->bsector, cWall->bwall, sx, fx, sy, fy, NULL });
        }
    }
    return;
}

static void render() {
    // 1. Get current sector
    // 2. Iterate through sector walls
    //      Render each wall using raycast method
    // 3. Check if any portals are visible
    //      If so, 4. Render each wall in adjacent sector
    //             5. Goto 3

    // Get the current sector
    int i;
    for (i = 0; i < state.sectors.n; ++i)
        if (point_in_sector((v2) { state.camera.pos.x, state.camera.pos.y }, (v2) { -0.6234242, -0.8813544 }, state.sectors.arr[i])) break;
    ASSERT(i < state.sectors.n, "CAMERA OUT OF BOUNDS %f %f", state.camera.pos.x, state.camera.pos.y);
    state.camera.sector = i;

    if (state.queue_head != NULL) free(state.queue_head);
    state.queue_head = malloc(sizeof(node_t));
    ASSERT(state.queue_head != NULL, "MEMERY FAILURE");
    set_node(state.queue_head, (node_t) { i, -1, (v2) { 0, 0 }, (v2) { 0, SCREEN_WIDTH }, (v2) { 0, SCREEN_HEIGHT }, (v2) { SCREEN_WIDTH, SCREEN_HEIGHT } });

    while (state.queue_head != NULL) {
        node_t* top = dequeue(&state.queue_head);
        if (top->sector == -1) break;
        render_sector(top->sector, top->a, top->b, top->c, top->d, top->via);
    }

    memset(state.pixels + SCREEN_WIDTH * SCREEN_HEIGHT / 2 + SCREEN_WIDTH / 2 - 7, 0xFF, 15); // temp crosshair
}

int main(int argc, char* argv[]) {
    char value = ' ';
    printf("Launch file creator? (Y/N) ");
    int a = scanf(" %c", &value);
    if (value == 'Y') {
        file_creator();
        return 0;
    }

    SDL_SetMainReady();

    ASSERT(!SDL_Init(SDL_INIT_VIDEO), "SDL failed to initialize: %s\n", SDL_GetError());

    state.window = SDL_CreateWindow("DEMO", SDL_WINDOWPOS_CENTERED_DISPLAY(0), SDL_WINDOWPOS_CENTERED_DISPLAY(0), 1280, 720, SDL_WINDOW_ALLOW_HIGHDPI);
    ASSERT(state.window, "failed to create SDL window: %s\n", SDL_GetError());

    state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_PRESENTVSYNC);
    ASSERT(state.renderer, "failed to create SDL renderer: %s\n", SDL_GetError());

    state.texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    ASSERT(state.texture, "failed to create SDL texture: %s\n", SDL_GetError());

    state.camera.pos = (v2){ 2, 2 };
    state.camera.angle = PI_4;
    state.pixels = malloc(sizeof(uint32_t) * SCREEN_WIDTH * SCREEN_HEIGHT);
    ASSERT(state.pixels != NULL, "MEMORY FAILURE");
    state.colormode = 1;

    struct map {
        struct sector_group sectors;
        struct wall_group walls;
    };

    struct map loaded_map;
    FILE* ptr;
    ptr = fopen("map.bin", "rb");  // r for read, b for binary
    fread(&loaded_map, sizeof(loaded_map), 1, ptr);

    state.sectors = loaded_map.sectors;
    state.walls = loaded_map.walls;

    while (!state.quit) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
            case SDL_QUIT:
                state.quit = true;
                break;
            }
        }

        const float rotspeed = 1.2f * 0.016f;
        const float movespeed = 1.2f * 0.016f;

        const uint8_t* keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_RIGHT]) {
            state.camera.angle -= rotspeed;
            state.camera.angle = normalize_a(state.camera.angle);
        }

        if (keystate[SDL_SCANCODE_LEFT]) {
            state.camera.angle += rotspeed;
            state.camera.angle = normalize_a(state.camera.angle);
        }

        if (keystate[SDL_SCANCODE_UP]) {
            state.camera.pos = (v2){
                state.camera.pos.x - (movespeed * cosf(state.camera.angle - PI_4)),
                state.camera.pos.y - (movespeed * sinf(state.camera.angle - PI_4)),
            };
        }

        if (keystate[SDL_SCANCODE_DOWN]) {
            state.camera.pos = (v2){
                state.camera.pos.x + (movespeed * cosf(state.camera.angle - PI_4)),
                state.camera.pos.y + (movespeed * sinf(state.camera.angle - PI_4)),
            };
        }

        memset(state.pixels, 0, sizeof(state.pixels));
        render();

        SDL_UpdateTexture(state.texture, NULL, state.pixels, SCREEN_WIDTH * sizeof(Uint32));
        SDL_RenderCopyEx(state.renderer, state.texture, NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL);
        SDL_RenderPresent(state.renderer);
    }

    SDL_DestroyTexture(state.texture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    return 0;
}