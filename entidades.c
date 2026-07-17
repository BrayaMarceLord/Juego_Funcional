#include "entidades.h"

void img_setPixel(imagen16x32_t *img, int8_t x, int8_t y)
{
    if(x < 0 || x >= CAMPO_ANCHO || y < 0 || y >= CAMPO_ALTO) return;
    img->cols[x] |= ((uint32_t)1 << y);
}

void img_clear(imagen16x32_t *img)
{
    uint8_t i;
    for(i = 0; i < 16; i++) img->cols[i] = 0x00000000;
}

// ------------------- PLAYER -------------------
// Nave de 3 px: punta en (x, 30), base en (x-1..x+1, 31)

void player_init(player_t *ptr)
{
    ptr->x = CAMPO_ANCHO / 2;
    ptr->vidas = 3;
}

void player_mover(player_t *ptr, int8_t dir)
{
    ptr->x += dir;
    // Limites (igual que rect.left/right en tu Python)
    if(ptr->x < 1) ptr->x = 1;
    if(ptr->x > CAMPO_ANCHO - 2) ptr->x = CAMPO_ANCHO - 2;
}

void player_draw(player_t *ptr, imagen16x32_t *img)
{
    img_setPixel(img, ptr->x,     CAMPO_ALTO - 2);   // punta
    img_setPixel(img, ptr->x - 1, CAMPO_ALTO - 1);   // base
    img_setPixel(img, ptr->x,     CAMPO_ALTO - 1);
    img_setPixel(img, ptr->x + 1, CAMPO_ALTO - 1);
}

// ------------------- BALAS -------------------

void balas_init(bala_t *pool)
{
    uint8_t i;
    for(i = 0; i < MAX_BALAS; i++) pool[i].activa = 0;
}

void balas_disparar(bala_t *pool, player_t *p)
{
    uint8_t i;
    for(i = 0; i < MAX_BALAS; i++)
    {
        if(!pool[i].activa)
        {
            pool[i].x = p->x;
            pool[i].y = CAMPO_ALTO - 3;   // sale de la punta de la nave
            pool[i].activa = 1;
            return;                        // una bala por disparo
        }
    }
}

void balas_update(bala_t *pool)
{
    uint8_t i;
    for(i = 0; i < MAX_BALAS; i++)
    {
        if(pool[i].activa)
        {
            pool[i].y--;                          // Python: speedy = -10
            if(pool[i].y < 0) pool[i].activa = 0; // Python: self.kill()
        }
    }
}

void balas_draw(bala_t *pool, imagen16x32_t *img)
{
    uint8_t i;
    for(i = 0; i < MAX_BALAS; i++)
        if(pool[i].activa) img_setPixel(img, pool[i].x, pool[i].y);
}

// ------------------- ENEMIGOS -------------------
// Figura 3x3 aleatoria con esquina superior-izquierda en (x, y)

// filas de arriba a abajo; bits = columnas (bit0=izq, bit1=centro, bit2=der)
static const uint8_t formas3x3[4][3] = {
    {0x07,0x07,0x07},  // 0 CUADRO   ### / ### / ###
    {0x02,0x05,0x02},  // 1 CIRCULO  .#. / #.# / .#.
    {0x02,0x07,0x02},  // 2 CRUZ (+) .#. / ### / .#.
    {0x05,0x02,0x05},  // 3 EQUIS(X) #.# / .#. / #.#
};

void enemigos_init(enemigo_t *pool)
{
    uint8_t i;
    for(i = 0; i < MAX_ENEMIGOS; i++) pool[i].activo = 0;
}

void enemigos_spawn(enemigo_t *pool, lfsr_t *rng)
{
    uint8_t i;
    for(i = 0; i < MAX_ENEMIGOS; i++)
    {
        if(!pool[i].activo)
        {
            pool[i].x = (int8_t)lfsr_rango(rng, CAMPO_ANCHO - 2); // 0..13 (cabe el 3x3)
            pool[i].y = 0;                                        // arriba
            pool[i].forma = (uint8_t)lfsr_rango(rng, 4);          // 0..3 aleatorio
            pool[i].activo = 1;
            return;
        }
    }
}

void enemigos_update(enemigo_t *pool)
{
    uint8_t i;
    for(i = 0; i < MAX_ENEMIGOS; i++)
        if(pool[i].activo) pool[i].y++;    // desciende; el game decide si llego abajo
}

void enemigos_draw(enemigo_t *pool, imagen16x32_t *img)
{
    uint8_t i, f, c;
    for(i = 0; i < MAX_ENEMIGOS; i++)
    {
        if(!pool[i].activo) continue;
        for(f = 0; f < 3; f++)
            for(c = 0; c < 3; c++)
                if(formas3x3[pool[i].forma][f] & (1 << c))
                    img_setPixel(img, pool[i].x + c, pool[i].y + f);
    }
}
