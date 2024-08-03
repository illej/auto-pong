#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> // rand ()

#include <SDL2/SDL.h>

#include "util.h"
#include "trace.h"
#include "map.h"
#include "vector2.h"


#define FRAME_TIME_MS   (1000.0f / 60.0f)
#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600
#define BUFFER_WIDTH    64
#define BUFFER_HEIGHT   32
#define BLOCK_SIZE_PX   30


struct entity
{
    v2 pos;
    v2 size;
    float radius; // BALL only
    v2 velocity;

    enum entity_type
    {
        E_TYPE_INVALID = 0,
        E_TYPE_WALL,
        E_TYPE_BLOCK,
        E_TYPE_BALL
    } type;

    enum team
    {
        E_TEAM_NONE = 0,
        E_TEAM_LIGHT,
        E_TEAM_DARK
    } team;

    u32 colour;
};

struct game
{
    u32 *buffer;
    int pitch;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    u32 n_entities;
    struct entity entities[512];

    u32 n_players;
    struct entity *players[2];

    float dt;
};

enum direction
{
    UP = 0,
    RIGHT,
    DOWN,
    LEFT
};

struct collision
{
    bool hit;
    enum direction direction;
    v2 vector;
};

/*
 * Globals
 */
static volatile bool running;


static char *
colour_str (struct entity *e)
{
    switch (e->team)
    {
        case E_TEAM_LIGHT: { return "Light"; }
        case E_TEAM_DARK:  { return "Dark"; }
        default:           { return "None"; }
    }
}

static char *
direction_str (enum direction dir)
{
    switch (dir)
    {
        case UP:    { return "Up"; }
        case LEFT:  { return "Left"; }
        case RIGHT: { return "Right"; }
        case DOWN:  { return "Down"; }
        default:    { return "N/A"; }
    }
}

static u32
team_colour (struct entity *e)
{
    u32 colour = 0x0;

    switch (e->team)
    {
        case E_TEAM_LIGHT:
            colour = 0xEEEEEE00;
            break;
        case E_TEAM_DARK:
            colour = 0x33333300;
            break;
        default:
            colour = 0xFFFFFF00;
    }

    return colour;
}

static char *
type_str (struct entity *e)
{
    switch (e->type)
    {
        case E_TYPE_WALL:  { return "Wall"; }
        case E_TYPE_BLOCK: { return "Block"; }
        case E_TYPE_BALL:  { return "Player"; }
        default:           { return "--"; }
    }
}

static float
randf (float min, float max)
{
    float r = rand ();
    return (r / RAND_MAX) * (max - min) + min;
}

static void
add_entity (struct game *game, enum entity_type type, int x, int y, enum team team)
{
    if (game->n_entities < LEN (game->entities))
    {
        struct entity *e = &game->entities[game->n_entities++];

        e->pos.x = x;
        e->pos.y = y;
        e->size.w = 1.0f;
        e->size.h = 1.0f;
        e->type = type;
        e->team = team;
        e->colour = team_colour (e);

        if (e->type == E_TYPE_BALL)
        {
            game->players[game->n_players++] = e;
            e->velocity.x = randf (-10.0f, 10.0f);
            e->velocity.y = randf (-10.0f, 10.0f);
            e->radius = 0.5f;

            printf ("Add player [@ x:%f y:%f] %s:%s\n",
                    e->pos.x, e->pos.y, type_str (e), colour_str (e));
        }
    }
    else
    {
        fprintf (stderr, "Failed to add entity\n");
    }
}

static void
init (struct game *game)
{
    game->buffer = (u32 *) malloc (WINDOW_WIDTH * WINDOW_HEIGHT * sizeof (u32));
    game->dt = 1.0f / 60.0f;
    game->pitch = sizeof (u32) * WINDOW_WIDTH; // u32 is 4 bytes :'(

    printf ("initialising SDL\n");

    SDL_Init (SDL_INIT_VIDEO);
    game->window = SDL_CreateWindow ("auto-pong",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH, WINDOW_HEIGHT,
            SDL_WINDOW_BORDERLESS);
    ASSERT (game->window);

    game->renderer = SDL_CreateRenderer (game->window, -1, SDL_RENDERER_ACCELERATED);
    ASSERT (game->renderer);

    game->texture = SDL_CreateTexture (game->renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            WINDOW_WIDTH, WINDOW_HEIGHT);
    ASSERT (game->texture);

    printf ("Loading map\n");
    /*
     * Load map
     *
     * TODO: not sure where this should go? the bits need to be defined somewhere i guess
     */
    for (int y = 0; y < 18; y++)
    {
        for (int x = 0; x < 18; x++)
        {
            unsigned int block = map[y][x];
            u8 type = block & 0x0F;
            u8 team = (block & 0xF0) >> 4;

            switch (team)
            {
                case 0x1:
                    team = E_TEAM_LIGHT;
                    break;
                case 0x2:
                    team = E_TEAM_DARK;
                    break;
            }

            switch (type)
            {
                case 0x1:
                    add_entity (game, E_TYPE_WALL, x, y, E_TEAM_NONE);
                    break;
                case 0x2:
                    add_entity (game, E_TYPE_BLOCK, x, y, team);
                    break;
                case 0x4:
                    add_entity (game, E_TYPE_BALL, x, y, team);
                    add_entity (game, E_TYPE_BLOCK, x, y, team == 0x1 ? E_TEAM_DARK : E_TEAM_LIGHT);
                    break;
            }
        }
    }

    printf ("Added %d entities\n", game->n_entities);
}

static void
handle_input (void)
{
    SDL_Event e;

    while (SDL_PollEvent (&e) != 0)
    {
        switch (e.type)
        {
            case SDL_QUIT:
                printf ("quit\n");
                running = false;
                break;
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    printf("esc\n");
                    running = false;
                }
                break;
        }
    }
}

#if 0
static void
draw_rect (struct game *game, int x, int y, u32 colour)
{

    // map = 18x18 blocks
    // window is 800x600 pixels
    //
    // block = 30 pixels
    // 18 * 30 = 540
    int window_half_w = WINDOW_WIDTH / 2;
    int map_size = 18 * 30;
    int map_half_w = map_size / 2;
    int offset = window_half_w - map_half_w;

    u32 *ptr = &game->buffer[offset + y * WINDOW_WIDTH + x];
    *ptr = colour;
}
#endif

static void
draw_circle (SDL_Renderer *renderer, float centreX, float centreY, float radius)
{
    const float diameter = (radius * 2);

    float x = (radius - 1);
    float y = 0;
    float tx = 1;
    float ty = 1;
    float error = (tx - diameter);

    while (x >= y)
    {
        //  Each of the following renders an octant of the circle
        SDL_RenderDrawPoint (renderer, centreX + x, centreY - y);
        SDL_RenderDrawPoint (renderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint (renderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint (renderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint (renderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint (renderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint (renderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint (renderer, centreX - y, centreY + x);

        if (error <= 0)
        {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0)
        {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

static bool
detect_hit_aabb (struct entity *a, v2 new_p, struct entity *b)
{
    bool hit = false;

    // Simple AABB collision detection
    if (new_p.x + 1 >= b->pos.x     &&
        new_p.x     <  b->pos.x + 1 &&
        new_p.y + 1 >= b->pos.y     &&
        new_p.y     <  b->pos.y + 1)
    {
        hit = true;
    }

    return hit;
}

static bool
collision_detect (struct entity *a, struct entity *b, struct collision *collision)
{
    bool hit = false;

    // get center of circle (assumes .pos is the top-left point)
    v2 a_centre = v2_addf (a->pos, a->radius);

    // calculate target AABB info (centre, half-extents)
    v2 b_half_extents = v2_divf (b->size, 2.0f);
    v2 b_centre = v2_add (b->pos, b_half_extents);

    // get difference vector between both centres
    v2 diff = v2_sub (a_centre, b_centre);

    v2 clamped = v2_clamp (diff, v2_neg (b_half_extents), b_half_extents);

    // add clamped value to b_centre and we get the value of box closest to circle
    v2 closest = v2_add (b_centre, clamped);

    // retrieve vector between centre circle and closest point on B and check if
    // length <= radius
    diff = v2_sub (closest, a_centre);

    hit = v2_len (diff) < a->radius;

    if (hit)
    {
        collision->hit = true;
        collision->direction = vector2_direction (diff);
        collision->vector = diff;
    }

    return hit;
}

static void
draw_rect (struct game *game, struct entity *e)
{
    SDL_Rect r = {0};

    r.x = e->pos.x * BLOCK_SIZE_PX;
    r.y = e->pos.y * BLOCK_SIZE_PX;
    r.w = BLOCK_SIZE_PX;
    r.h = BLOCK_SIZE_PX;

    if (e->team == E_TEAM_LIGHT)
    {
        SDL_SetRenderDrawColor (game->renderer, 0xEE, 0xEE, 0xEE, 0xFF);
    }
    else if (e->team == E_TEAM_DARK)
    {
        SDL_SetRenderDrawColor (game->renderer, 0x33, 0x33, 0x33, 0xFF);
    }
    else
    {
        SDL_SetRenderDrawColor (game->renderer, 0xFF, 0x11, 0x11, 0xFF);
    }

    SDL_RenderFillRect (game->renderer, &r);
}

static enum direction
vector2_direction (v2 target)
{
    v2 compass[] = {
        [UP]    = {  0.0f, -1.0f },
        [RIGHT] = {  1.0f,  0.0f },
        [DOWN]  = {  0.0f,  1.0f },
        [LEFT]  = { -1.0f,  0.0f }
    };
    float max = 0.0f;
    u32 best_match = UINT32_MAX;

    for (int i = 0; i < LEN (compass); i++)
    {
        float dot_product = v2_inner (v2_norm (target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }

    return best_match;
}

static void
collision_resolve (struct entity *a, struct entity *b, struct collision *collision)
{
    enum direction dir = collision->direction;
    v2 diff = collision->vector;

    if (dir == LEFT || dir == RIGHT)
    {
        a->velocity.x = -a->velocity.x;
    }
    else
    {
        a->velocity.y = -a->velocity.y;
    }
}

static void
update (struct game *game)
{
    for (int i = 0; i < game->n_players; i++)
    {
        struct entity *p = game->players[i];
        struct entity *hit = NULL;

        for (int j = 0; j < game->n_entities; j++)
        {
            struct entity *test = &game->entities[j];
            struct collision collision = {0};

            if (collision_detect (p, test, &collision))
            {
                if (test->type == E_TYPE_WALL)
                {
                    hit = test;
                }
                else if (test->type == E_TYPE_BLOCK)
                {
                    if (test->team == p->team)
                    {
                        // continue on
                        if (p->team == E_TEAM_LIGHT)
                        {
                            test->team = E_TEAM_DARK;
                        }
                        else if (p->team == E_TEAM_DARK)
                        {
                            test->team = E_TEAM_LIGHT;
                        }

                        hit = test;
                    }
                }
            }

            if (hit)
            {
                collision_resolve (p, hit, &collision);
                break;
            }
        }

        p->pos.x += p->velocity.x * game->dt;
        p->pos.y += p->velocity.y * game->dt;
    }

    for (int i = 0; i < game->n_entities; i++)
    {
        struct entity *e = &game->entities[i];
        if (e->type != E_TYPE_BALL)
        {
            draw_rect (game, e);
        }
    }

    for (int i = 0; i < game->n_players; i++)
    {
        struct entity *e = game->players[i];

        if (e->team == E_TEAM_LIGHT)
        {
            SDL_SetRenderDrawColor (game->renderer, 0xEE, 0xEE, 0xEE, 0xFF);
        }
        else if (e->team == E_TEAM_DARK)
        {
            SDL_SetRenderDrawColor (game->renderer, 0x33, 0x33, 0x33, 0xFF);
        }
        else
        {
            SDL_SetRenderDrawColor (game->renderer, 0xFF, 0x11, 0x11, 0xFF);
        }

        draw_circle (game->renderer, (e->pos.x + e->radius) * BLOCK_SIZE_PX, (e->pos.y + e->radius) * BLOCK_SIZE_PX, e->radius * BLOCK_SIZE_PX);
    }
}

static void
signal_handler (int signal)
{
    printf ("signal received: %d\n", signal);
    switch (signal)
    {
        case SIGINT:
            running = false;
            break;
        case SIGSEGV:
            printf ("SEGMENTATION FAULT\n");
            stack_trace ();
            running = false;
            break;
    }
}

static void
trace_init (void)
{
    bool ok = (signal (SIGINT, signal_handler) != SIG_ERR &&
               signal (SIGSEGV, signal_handler) != SIG_ERR);
    if (!ok)
    {
        fprintf (stderr, "Failed to initialise signal handler\n");
    }
}

int
main (int argc, char **argv)
{
    struct game game = {0};

    srand (117); // Use the same seed

    trace_init ();
    init (&game);

    running = true;
    while (running)
    {
        handle_input ();

        SDL_SetRenderDrawColor (game.renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear (game.renderer);

        update (&game);

        SDL_RenderPresent (game.renderer);
        SDL_Delay (FRAME_TIME_MS);
    }

    return 0;
}
