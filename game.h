#ifndef GAME_SHOOTER
#define GAME_SHOOTER

    #include "entidades.h"
    #include "botones.h"
    #include "3_HardwareSupport/display16x32.h"
    #include "1_Drivers/LEDS.h"
    #include "1_Drivers/Driver_UART.h"
    #include "2_HAL/lcd_i2c.h"

    #define TICK_HZ       30
    #define NIVEL_TICKS   (2u * 60u * TICK_HZ)   // 2 minutos por nivel

    typedef enum{
        ST_INICIO,        // Python: show_go_screen()
        ST_MENU,          // seleccion de dificultad
        ST_JUGANDO,
        ST_PAUSA,
        ST_SCORE_NIVEL,   // pantalla de score al terminar nivel
        ST_GAME_OVER,     // Python: game_over_scene()
        ST_VICTORIA,      // 3 niveles completados
    }gameState_t;

    typedef enum{ FACIL = 0, NORMAL = 1, DIFICIL = 2 }dificultad_t;

    typedef struct{
        uint8_t velEnemigos;   // ticks entre pasos de descenso
        uint8_t spawnPeriodo;  // ticks entre apariciones
        uint8_t maxEnemigos;   // simultaneos
    }nivelCfg_t;

    typedef struct{
        gameState_t estado;
        dificultad_t dificultad;
        uint8_t nivel;             // 0..2
        uint16_t score;
        uint32_t tickNivel;        // cuenta hasta NIVEL_TICKS
        uint8_t divBalas;          // divisores de velocidad
        uint8_t divEnemigos;
        uint8_t divSpawn;
        uint8_t cursorMenu;        // 0..2 en ST_MENU
        uint16_t scorePrev;        // para refrescar la LCD solo al cambiar
        uint8_t vidasPrev;
        player_t nave;
        bala_t balas[MAX_BALAS];
        enemigo_t enemigos[MAX_ENEMIGOS];
        lfsr_t rng;
        // Punteros a las capas del profesor:
        display16x32_t *display;
        botones_t *botones;
        LEDS_t *leds;
        Driver_Uart_t *uart;
        lcd_t *lcd;
    }game_t;

    extern void game_init(game_t *g);
    extern void game_tick(game_t *g);   // llamar a TICK_HZ desde el main

#endif