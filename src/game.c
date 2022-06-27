#include <stdlib.h> // for rand
#include "game.h"
#include "app.h"
#include "starfield.h"
#include "renderer.h"

// TODO(shaw): handle floating point for 16.666667 (60fps)
#define TIME_STEP 16  // milliseconds per frame
#define FPS ((int)(1.0f/(TIME_STEP/1000.0f)))  

typedef enum {
    SIDE_PLAYER,
    SIDE_ENEMY
} Side;

typedef struct Entity Entity;

struct Entity {
    float x, y, vx, vy;
    int w, h;
    int reload;
    float speed;
    int health;
    Side side;
    SDL_Texture *texture;
    Entity *next;
};

typedef struct Explosion Explosion;

struct Explosion {
    float x, y;
    float vx, vy;
    int r,g,b,a;
    Explosion *next;
};

typedef struct Debris Debris;

struct Debris {
	float x;
	float y;
	float vx;
	float vy;
	SDL_Rect rect;
	SDL_Texture *texture;
	int life;
	Debris *next;
};


Entity ship_head          = {0};
Entity *ship_tail         = {0};
Entity bullet_head        = {0};
Entity *bullet_tail       = {0};
Explosion explosion_head  = {0};
Explosion *explosion_tail = {0};
Debris debris_head        = {0};
Debris *debris_tail       = {0};

Entity player = {0};

SDL_Texture *bullet_texture       = {0};
SDL_Texture *enemy_bullet_texture = {0};
SDL_Texture *enemy_texture        = {0};
SDL_Texture *explosion_texture    = {0};

static int bullet_width, bullet_height;
static int reset_timer;

static void game_reset(void);
static void fire_bullet(void);
static void player_update(void);
static void bullets_update(void);
static void spawn_enemies(void);
static void ships_update(void);
static bool bullet_hit_ship(Entity *b);
static bool bullet_hit_player(Entity *b);
static bool collide(Entity *a, Entity *b);
static bool player_hit_ship();
static void enemy_fire_bullet(Entity *e);

static void add_explosions(int x, int y, int num);
static void explosions_update(void);
static void explosions_render(void);

static void add_debris(Entity *e);
static void debris_update(void);
static void debris_render(void);


void game_init(void) {
    player.speed = 4.0f;
    player.side = SIDE_PLAYER;
    player.texture = load_texture("assets/player.png");
	SDL_QueryTexture(player.texture, NULL, NULL, &player.w, &player.h);

    enemy_texture = load_texture("assets/enemy.png");
    enemy_bullet_texture = load_texture("assets/alienBullet.png");

    bullet_texture = load_texture("assets/playerBullet.png");
	SDL_QueryTexture(bullet_texture, NULL, NULL, &bullet_width, &bullet_height);

    explosion_texture = load_texture("assets/explosion.png");
    SDL_SetTextureBlendMode(explosion_texture, SDL_BLENDMODE_ADD);

    game_reset();
}

static void game_reset(void) {
    player.x = 100;
    player.y = 100;
    player.health = 3;

    Entity *e;
    Explosion *ex;
    Debris *d;

    // delete bullets
    while (bullet_head.next) {
        e = bullet_head.next;
        free(e);
        bullet_head.next = e->next;
    }
    bullet_tail = &bullet_head;

    // delete ships
    while (ship_head.next) {
        e = ship_head.next;
        free(e);
        ship_head.next = e->next;
    }
    ship_tail = &ship_head;

    // delete explosions
    while (explosion_head.next) {
        ex = explosion_head.next;
        free(ex);
        explosion_head.next = ex->next;
    }
    explosion_tail = &explosion_head;

    // delete debris
    while (debris_head.next) {
        d = debris_head.next;
        free(d);
        debris_head.next = d->next;
    }
    debris_tail = &debris_head;

    starfield_init();

    reset_timer = FPS * 2;
}

void game_update(void) {
    static uint64_t accumulator = 0;
    static uint64_t prev_time = 0;
    static uint64_t cur_time = 0;
    static uint64_t delta = 0;

    cur_time = SDL_GetTicks64();
    delta = cur_time - prev_time;

    accumulator += delta;
    while (accumulator > TIME_STEP) {
        player_update();
        ships_update();
        bullets_update();
        spawn_enemies();
        explosions_update();
        debris_update();
        starfield_update();
        if (player.health <= 0 && reset_timer-- < 0)
            game_reset();
        accumulator -= TIME_STEP;
    }

    prev_time = cur_time;
}

void game_render(void) {
    starfield_render();

    // draw ships
    Entity *e = &ship_head;
    for (e = e->next; e; e = e->next)
        blit(e->texture, e->x, e->y);

    // draw player
    if (player.health > 0)
        blit(player.texture, player.x, player.y);

    // draw bullets
    Entity *b = &bullet_head;
    for (b = b->next; b; b = b->next)
        blit(b->texture, b->x, b->y);

    debris_render();
    explosions_render();
}


static void ships_update(void) {
    Entity *e = &ship_head;
    Entity *prev = e;
    for (e = e->next; e; e = e->next) {
        e->x += e->vx;
        e->y += e->vy;

        if (e->x < -e->w || e->health <= 0) {
            if (e == ship_tail)
                ship_tail = prev;
            prev->next = e->next;
            free(e);
            e = prev;
        }

        if (e->reload-- <= 0) {
            enemy_fire_bullet(e);
            e->reload = 100;
        }

        prev = e;
    }
}

static void player_update(void) {
    if (player.health <= 0) return;

    player.vx = player.vy = 0;

    // movement
    if (app.right && !app.left)
        player.x += player.speed;         
    else if (app.left && !app.right)
        player.x -= player.speed;         

    if (player.x <= 0) player.x = 0;
    else if (player.x+player.w >= SCREEN_WIDTH) player.x = SCREEN_WIDTH - player.w;

    if (app.up && !app.down)
        player.y -= player.speed;         
    else if (app.down && !app.up)
        player.y += player.speed;         

    if (player.y <= 0) player.y = 0;
    else if (player.y+player.h >= SCREEN_HEIGHT) player.y = SCREEN_HEIGHT - player.h;

    // shooting
    if (app.fire && player.reload <= 0) {
        fire_bullet();
        player.reload = 8;
    }
    if (player.reload-- < 0) player.reload = 0;

    // collide with enemies
    if (player_hit_ship()) {
        // do some effect here
    }
}

static void fire_bullet(void) {
    Entity *b = calloc(1, sizeof(Entity));
    b->w = bullet_width;
    b->h = bullet_height;
    b->x = player.x + 0.8*player.w - 0.5*b->w; 
    b->y = player.y + 0.5*player.h - 0.5*b->h;
    b->vx = 8.0;
    b->health = 1;
    b->side = SIDE_PLAYER;
    b->texture = bullet_texture;
    bullet_tail->next = b;
    bullet_tail = b;
}

static void enemy_fire_bullet(Entity *e) {
    Entity *b = calloc(1, sizeof(Entity));
    SDL_QueryTexture(enemy_bullet_texture, NULL, NULL, &b->w, &b->h);
    b->x = e->x + 0.2*e->w - 0.5*b->w;
    b->y = e->y + 0.5*e->h - 0.5*b->h;
    b->vx = e->vx + -(2 + rand() % 7);
    b->health = 1;
    b->side = SIDE_ENEMY;
    b->texture = enemy_bullet_texture;
    bullet_tail->next = b;
    bullet_tail = b;
}

static bool player_hit_ship() {
    Entity *e = &ship_head;
    for (e = e->next; e; e = e->next) {
        if (e->side != player.side && collide(&player, e)) {
            if (--player.health == 0) {
                add_explosions(player.x, player.y, 32);
                add_debris(&player);
            }
            if (--e->health == 0) {
                add_explosions(e->x, e->y, 32);
                add_debris(e);
            }
            return true;
        }
    }
    return false;
}

static void bullets_update(void) {
    Entity *b = &bullet_head;
    Entity *prev = b;

    for (b = b->next; b; b = b->next) {
        b->x += b->vx;
        b->y += b->vy;

        if (bullet_hit_ship(b) || bullet_hit_player(b) || b->x < -b->w || b->x > SCREEN_WIDTH) {
            if (b == bullet_tail)
                bullet_tail = prev;
            prev->next = b->next;
            free(b);
            b = prev;
        }
        prev = b;
    }
}

static bool bullet_hit_ship(Entity *b) {
    Entity *e = &ship_head;
    for (e = e->next; e; e = e->next) {
        if (b->side != e->side && collide(b, e)) {
            --b->health;
            if (--e->health == 0) {
                add_explosions(e->x, e->y, 32);
                add_debris(e);
            }
            return true;
        }
    }
    return false;
}

static bool bullet_hit_player(Entity *b) {
    if (b->side != player.side && collide(b, &player)) {
        --b->health;
        if (--player.health == 0) {
            add_explosions(player.x, player.y, 32);
            add_debris(&player);
        }
        return true;
    }
    return false;
}

static bool collide(Entity *a, Entity *b) {
    return (MAX(a->x, b->x) < MIN(a->x + a->w, b->x + b->w)) && 
           (MAX(a->y, b->y) < MIN(a->y + a->h, b->y + b->h));
}

static void spawn_enemies(void) {
    static int spawn_timer;

    if (--spawn_timer <= 0) {
        spawn_timer = 30 + (rand() % 60);
        Entity *e = calloc(1, sizeof(Entity));
        SDL_QueryTexture(enemy_texture, NULL, NULL, &e->w, &e->h);
        e->x = SCREEN_WIDTH;
        e->y = rand() % SCREEN_HEIGHT;
        while (e->y + e->h > SCREEN_HEIGHT) 
            e->y = rand() % SCREEN_HEIGHT;
        e->vx = -(2 + (rand() % 4));
        e->health = 1;
        e->side = SIDE_ENEMY;
        e->texture = enemy_texture;
        ship_tail->next = e;
        ship_tail = e;
    }
}

//////////////////////////////////////////////////////////////////////////////
// EXPLOSIONS
//////////////////////////////////////////////////////////////////////////////
static void add_explosions(int x, int y, int num) {
    for (int i=0; i<num; ++i) {
        Explosion *e = calloc(1, sizeof(Explosion));
        e->x  = x + (rand()%32 - rand()%32);
        e->y  = y + (rand()%32 - rand()%32);
        e->vx = 0.1 * (rand()%10 - rand()%10);
        e->vy = 0.1 * (rand()%10 - rand()%10);

		switch (rand() % 4) {
            case 0:  e->r = 255; break;
            case 1:  e->r = 255; e->g = 128; break;
            case 2:  e->r = 255; e->g = 255; break;
            default: e->r = 255; e->g = 255; e->b = 255; break;
		}

        e->a = rand() % (FPS*2);

        explosion_tail->next = e;
        explosion_tail = e;
    }
}

static void explosions_update(void) {
    Explosion *e = &explosion_head;
    Explosion *prev = e;
    for (e = e->next; e; e = e->next) {
        e->x += e->vx;
        e->y += e->vy;
        if (e->a-- <= 0) {
            if (e == explosion_tail)
                explosion_tail = prev;
            prev->next = e->next;
            free(e);
            e = prev;
        }
        prev = e;
    }
}

static void explosions_render(void) {
    Explosion *e = &explosion_head;

	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_ADD);

    for (e = e->next; e; e = e->next) {
        SDL_SetTextureColorMod(explosion_texture, e->r, e->g, e->b);
		SDL_SetTextureAlphaMod(explosion_texture, e->a);
		blit(explosion_texture, e->x, e->y); 
    }

	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
}

//////////////////////////////////////////////////////////////////////////////
// DEBRIS
//////////////////////////////////////////////////////////////////////////////
static void add_debris(Entity *e) {
    int rows = 2;
    int cols = 2;
    int row_h = e->h/rows;
    int col_w = e->w/rows;

    for (int row=0; row<rows; ++row)
    for (int col=0; col<cols; ++col)
    {
        Debris *d = calloc(1, sizeof(Debris));
        d->x  = e->x + 0.5*e->w;
        d->y  = e->y + 0.5*e->h;
        d->vx = 0.3 * (rand()%10 - rand()%10);
        d->vy = 0.3 * (rand()%10 - rand()%10);
        d->life = FPS*2;
        d->texture = e->texture;
        d->rect = (SDL_Rect) {
            .x = col * col_w,
            .y = row * row_h,
            .w = col_w,
            .h = row_h,
        };

        debris_tail->next = d;
        debris_tail = d;
    }
}

static void debris_update(void) {
    Debris *d = &debris_head;
    Debris *prev = d;
    for (d = d->next; d; d = d->next) {
        d->x += d->vx;
        d->y += d->vy;
        if (d->life-- <= 0) {
            if (d == debris_tail)
                debris_tail = prev;
            prev->next = d->next;
            free(d);
            d = prev;
        }
        prev = d;
    }
}

static void debris_render(void) {
    Debris *d = &debris_head;
    for (d = d->next; d; d = d->next) {
		SDL_SetTextureAlphaMod(d->texture, d->life);
        blit_rect(d->texture, &d->rect, d->x, d->y);
		SDL_SetTextureAlphaMod(d->texture, 255);
    }
}

