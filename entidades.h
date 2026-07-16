#ifndef ENTIDADES_GAME
#define ENTIDADES_GAME

    #include "stdint.h"
    #include "4_APIs/graphics.h"
    #include "lfsr.h"

    // Sistema de coordenadas sobre imagen16x32_t:
    //   x = indice de cols[] (0..15, ancho del campo)
    //   y = bit dentro de la palabra (0 = arriba, 31 = abajo)
    #define CAMPO_ANCHO   16
    #define CAMPO_ALTO    32
    #define MAX_BALAS      4
    #define MAX_ENEMIGOS   6

    typedef struct{           // Python: class Player
        int8_t x;             // columna del centro de la nave
        uint8_t vidas;
    }player_t;

    typedef struct{           // Python: class Bullet
        int8_t x;
        int8_t y;
        uint8_t activa;
    }bala_t;

    typedef struct{           // Python: class Meteor (cuadrado 2x2)
        int8_t x;             // esquina izquierda
        int8_t y;             // esquina superior
        uint8_t activo;
    }enemigo_t;

    // --- Player ---
    extern void player_init(player_t *ptr);
    extern void player_mover(player_t *ptr, int8_t dir);          // -1 izq, +1 der
    extern void player_draw(player_t *ptr, imagen16x32_t *img);

    // --- Balas (pool estatico, reemplaza a pygame.sprite.Group) ---
    extern void balas_init(bala_t *pool);
    extern void balas_disparar(bala_t *pool, player_t *p);        // Python: player.shoot()
    extern void balas_update(bala_t *pool);
    extern void balas_draw(bala_t *pool, imagen16x32_t *img);

    // --- Enemigos ---
    extern void enemigos_init(enemigo_t *pool);
    extern void enemigos_spawn(enemigo_t *pool, lfsr_t *rng);
    extern void enemigos_update(enemigo_t *pool);                 // descienden 1 px
    extern void enemigos_draw(enemigo_t *pool, imagen16x32_t *img);

    // --- Utilidad de dibujo ---
    extern void img_setPixel(imagen16x32_t *img, int8_t x, int8_t y);
    extern void img_clear(imagen16x32_t *img);

#endif