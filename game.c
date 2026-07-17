#include "game.h"
#include "ti_msp_dl_config.h"
#include "buzzer.h"
#include "sfx.h"

extern volatile uint16_t ADCLight;
extern uint8_t sensor_luzAIntensidad(uint16_t adc);

// -------- Tabla de dificultad: [dificultad][nivel] --------
static const nivelCfg_t tabla[3][3] = {
    // FACIL:   lento, pocos
    { {15, 75, 3}, {13, 65, 3}, {11, 55, 4} },
    // NORMAL
    { {10, 55, 4}, { 9, 45, 4}, { 8, 40, 5} },
    // DIFICIL: rapido, muchos
    { { 7, 40, 5}, { 6, 32, 6}, { 5, 25, 6} },
};

// -------- Fuente 3x5 para digitos 0-9 (3 columnas x 5 filas) --------
static const uint8_t font3x5[10][3] = {
    {0x1F,0x11,0x1F},{0x00,0x1F,0x00},{0x1D,0x15,0x17},{0x15,0x15,0x1F},
    {0x07,0x04,0x1F},{0x17,0x15,0x1D},{0x1F,0x15,0x1D},{0x01,0x01,0x1F},
    {0x1F,0x15,0x1F},{0x17,0x15,0x1F},
};

static void game_drawDigito(imagen16x32_t *img, uint8_t d, int8_t x, int8_t y)
{
    uint8_t c, f;
    for(c = 0; c < 3; c++)
        for(f = 0; f < 5; f++)
            if(font3x5[d][c] & (1 << f)) img_setPixel(img, x + c, y + f);
}

static void game_drawNumero(imagen16x32_t *img, uint16_t n, int8_t y)
{
    // Hasta 4 digitos centrados en las 16 columnas (4 px por digito)
    uint8_t dig[4] = {0,0,0,0};
    int8_t i, ndig = 0;
    if(n > 9999) n = 9999;
    do{ dig[ndig++] = n % 10; n /= 10; }while(n > 0 && ndig < 4);
    int8_t x = (16 - (ndig * 4 - 1)) / 2;
    for(i = ndig - 1; i >= 0; i--){ game_drawDigito(img, dig[i], x, y); x += 4; }
}

// -------- Vidas en los LEDs de la placa (1 LED por vida) --------
static void game_mostrarVidas(game_t *g)
{
    uint8_t i;
    for(i = 0; i < 4; i++)
    {
        if(i < g->nave.vidas)
            (g->leds->leds_port[i]) ? (GPIOB->DOUTSET31_0 = g->leds->leds[i])
                                    : (GPIOA->DOUTSET31_0 = g->leds->leds[i]);
        else
            (g->leds->leds_port[i]) ? (GPIOB->DOUTCLR31_0 = g->leds->leds[i])
                                    : (GPIOA->DOUTCLR31_0 = g->leds->leds[i]);
    }
}

static void game_scoreUART(game_t *g)
{
    char msg[24] = "Score: ";
    uint8_t i = 7;
    uint16_t s = g->score;
    char tmp[6]; int8_t n = 0;
    do{ tmp[n++] = '0' + (s % 10); s /= 10; }while(s > 0);
    while(n > 0) msg[i++] = tmp[--n];
    msg[i++] = '\r'; msg[i++] = '\n'; msg[i] = '\0';
    Driver_Uart_sendMessageUart(g->uart, msg);
}

// -------- Pantallas de la LCD 16x2 (I2C) --------
static void lcd_pantallaMenu(game_t *g)
{
    static const char *nombres[3] = {"FACIL  ", "NORMAL ", "DIFICIL"};
    lcd_clear(g->lcd);
    lcd_setCursor(g->lcd, 0, 0); lcd_print(g->lcd, "Dificultad:");
    lcd_setCursor(g->lcd, 0, 1); lcd_print(g->lcd, nombres[g->cursorMenu]);
}

static void lcd_pantallaJuego(game_t *g)
{
    lcd_clear(g->lcd);
    lcd_setCursor(g->lcd, 0, 0); lcd_print(g->lcd, "Score: ");
    lcd_printNum(g->lcd, g->score);
    lcd_setCursor(g->lcd, 0, 1); lcd_print(g->lcd, "Vidas: ");
    lcd_printNum(g->lcd, g->nave.vidas);
}

static void lcd_pantallaFin(game_t *g, const char *titulo)
{
    lcd_clear(g->lcd);
    lcd_setCursor(g->lcd, 0, 0); lcd_print(g->lcd, titulo);
    lcd_setCursor(g->lcd, 0, 1); lcd_print(g->lcd, "Score: ");
    lcd_printNum(g->lcd, g->score);
}

static void game_cargarNivel(game_t *g)
{
    g->tickNivel = 0;
    g->divBalas = 0; g->divEnemigos = 0; g->divSpawn = 0;
    balas_init(g->balas);
    enemigos_init(g->enemigos);
    player_init(&g->nave);           // recentra la nave...
    g->nave.vidas = g->nave.vidas;   // ...las vidas se conservan entre niveles
    g->scorePrev = 0xFFFF;           // fuerza refresco de la LCD en el primer tick
    g->vidasPrev = 0xFF;
}

void game_init(game_t *g)
{
    g->estado = ST_INICIO;
    g->cursorMenu = NORMAL;
    lfsr_init(&g->rng, 0xBEEF);
    player_init(&g->nave);
}

// ---------------- Logica por estado ----------------

static void game_tickJugando(game_t *g)
{
    const nivelCfg_t *cfg = &tabla[g->dificultad][g->nivel];
    uint8_t i, k;

    // 1) Entradas: MPU (inclinacion) + botones
    static uint8_t divMpu = 0;
    if(++divMpu >= 3)                     // lee el sensor a 10 Hz (I2C compartido con LCD)
    {
        divMpu = 0;
        mpu6050_axis_t a;
        if(mpu6050_readAccel(g->mpu, &a))
        {
            // ACCEL_FS_2G: 16384 LSB/g. Umbral ~15 grados: sin(15)*16384 = 4200
            if(a.y >  4200)      player_mover(&g->nave, +1);   // 10 px/s
            else if(a.y < -4200) player_mover(&g->nave, -1);
        }
    }

    if(botones_izquierda(g->botones))  player_mover(&g->nave, -1);
    if(botones_derecha(g->botones))    player_mover(&g->nave, +1);
    if(botones_flancoDisparo(g->botones)){
        balas_disparar(g->balas, &g->nave);
        buzzer_play(sfx_disparo, sfx_disparo_d, BUZZER_LEN(sfx_disparo), 120, false);
    }
    if(botones_flancoPausa(g->botones)){ g->estado = ST_PAUSA; buzzer_music_stop(); return; }

    // 2) Fisica (con divisores de frecuencia sobre el tick)
    if(++g->divBalas >= 2){ g->divBalas = 0; balas_update(g->balas); }         // 15 px/s
    if(++g->divEnemigos >= cfg->velEnemigos){ g->divEnemigos = 0; enemigos_update(g->enemigos); }
    if(++g->divSpawn >= cfg->spawnPeriodo)
    {
        g->divSpawn = 0;
        uint8_t activos = 0;
        for(i = 0; i < MAX_ENEMIGOS; i++) if(g->enemigos[i].activo) activos++;
        if(activos < cfg->maxEnemigos) enemigos_spawn(g->enemigos, &g->rng);
        lfsr_next(&g->rng);   // sigue mezclando la semilla
    }

    // 3) Colisiones bala-enemigo (figura 3x3: bounding box de +2)
    for(k = 0; k < MAX_BALAS; k++)
    {
        if(!g->balas[k].activa) continue;
        for(i = 0; i < MAX_ENEMIGOS; i++)
        {
            if(!g->enemigos[i].activo) continue;
            if(g->balas[k].x >= g->enemigos[i].x && g->balas[k].x <= g->enemigos[i].x + 2 &&
               g->balas[k].y >= g->enemigos[i].y && g->balas[k].y <= g->enemigos[i].y + 2)
            {
                g->balas[k].activa = 0;
                g->enemigos[i].activo = 0;
                g->score += 10;
                buzzer_play(sfx_impacto, sfx_impacto_d, BUZZER_LEN(sfx_impacto), 140, false);
                break;
            }
        }
    }

    // 4) Enemigo llega abajo o toca la nave -> -1 vida (figura 3x3: +2)
    for(i = 0; i < MAX_ENEMIGOS; i++)
    {
        if(!g->enemigos[i].activo) continue;
        uint8_t abajo = (g->enemigos[i].y + 2 >= CAMPO_ALTO - 1);
        uint8_t tocaNave = (g->enemigos[i].y + 2 >= CAMPO_ALTO - 2) &&
                           (g->enemigos[i].x + 2 >= g->nave.x - 1) &&
                           (g->enemigos[i].x     <= g->nave.x + 1);
        if(abajo || tocaNave)
        {
            g->enemigos[i].activo = 0;
            if(g->nave.vidas > 0){
                g->nave.vidas--;
                buzzer_play(sfx_vida, sfx_vida_d, BUZZER_LEN(sfx_vida), 110, false);
            }
            if(g->nave.vidas == 0){
                buzzer_music_stop();
                buzzer_play(sfx_gameover, sfx_gameover_d, BUZZER_LEN(sfx_gameover), 100, false);
                lcd_pantallaFin(g, "GAME OVER");
                g->estado = ST_GAME_OVER; game_scoreUART(g); return;
            }
        }
    }

    // 5) Tiempo del nivel
    if(++g->tickNivel >= NIVEL_TICKS)
    {
        buzzer_music_stop();
        game_scoreUART(g);
        g->estado = ST_SCORE_NIVEL;
        return;
    }

    // 6) Render (panel)
    img_clear(&g->display->myImage);
    player_draw(&g->nave, &g->display->myImage);
    balas_draw(g->balas, &g->display->myImage);
    enemigos_draw(g->enemigos, &g->display->myImage);
    game_mostrarVidas(g);

    // 7) LCD: refrescar solo cuando cambie score o vidas (I2C es lento)
    if(g->score != g->scorePrev || g->nave.vidas != g->vidasPrev){
        lcd_pantallaJuego(g);
        g->scorePrev = g->score;
        g->vidasPrev = g->nave.vidas;
    }
}

static void game_tickMenu(game_t *g)
{
    static uint8_t prevIzq = 0, prevDer = 0;
    uint8_t izq = botones_izquierda(g->botones);
    uint8_t der = botones_derecha(g->botones);

    if(izq && !prevIzq && g->cursorMenu > 0){ g->cursorMenu--; lcd_pantallaMenu(g); }
    if(der && !prevDer && g->cursorMenu < 2){ g->cursorMenu++; lcd_pantallaMenu(g); }
    prevIzq = izq; prevDer = der;

    if(botones_flancoDisparo(g->botones))
    {
        g->dificultad = (dificultad_t)g->cursorMenu;
        g->nivel = 0;
        g->score = 0;
        g->nave.vidas = 3;
        game_cargarNivel(g);
        g->estado = ST_JUGANDO;
        buzzer_music_play(notes, durations, BUZZER_LEN(notes), 90, true);  // musica de fondo en loop
        return;
    }

    // Dibujo: 1, 2 o 3 cuadros 4x4; el seleccionado relleno, los demas solo borde
    img_clear(&g->display->myImage);
    uint8_t d, f, c;
    for(d = 0; d <= 2; d++)
    {
        int8_t x0 = 2 + d * 4 + d;         // separados
        int8_t y0 = 14;
        for(f = 0; f < 4; f++)
            for(c = 0; c < 4; c++)
            {
                uint8_t borde = (f == 0 || f == 3 || c == 0 || c == 3);
                if((d == g->cursorMenu) || borde)
                    img_setPixel(&g->display->myImage, x0 + c, y0 + f);
            }
    }
}

void game_tick(game_t *g)
{
    static gameState_t estadoPrevSerial = ST_INICIO;

    switch(g->estado)
    {
        case ST_INICIO:
            // Cualquier boton entra al menu (Python: "presione cualquier tecla")
            img_clear(&g->display->myImage);
            player_draw(&g->nave, &g->display->myImage);      // muestra la nave como portada
            if(botones_flancoDisparo(g->botones) || botones_flancoPausa(g->botones) ||
               botones_izquierda(g->botones) || botones_derecha(g->botones)){
                lcd_pantallaMenu(g);        // pinta el menu al entrar
                g->estado = ST_MENU;
            }
            break;

        case ST_MENU:
            game_tickMenu(g);
            break;

        case ST_JUGANDO:
            game_tickJugando(g);
            break;

        case ST_PAUSA:
            // Frame congelado; parpadea el pixel (0,0) como indicador
            if(g->tickNivel % TICK_HZ < TICK_HZ/2)
                img_setPixel(&g->display->myImage, 0, 0);
            else
                g->display->myImage.cols[0] &= ~0x00000001;
            if(botones_flancoPausa(g->botones)){
                g->estado = ST_JUGANDO;
                g->scorePrev = 0xFFFF;   // fuerza redibujar la LCD al volver
                g->vidasPrev = 0xFF;
                buzzer_music_play(notes, durations, BUZZER_LEN(notes),90, true);
            }
            break;

        case ST_SCORE_NIVEL:
            img_clear(&g->display->myImage);
            game_drawNumero(&g->display->myImage, g->score, 13);
            lcd_pantallaFin(g, "Nivel completo");
            if(botones_flancoDisparo(g->botones))
            {
                if(g->nivel < 2){
                    g->nivel++; game_cargarNivel(g); g->estado = ST_JUGANDO;
                    buzzer_music_play(notes, durations, BUZZER_LEN(notes), 90, true);
                }
                else {
                    buzzer_music_stop();
                    lcd_pantallaFin(g, "VICTORIA!");
                    g->estado = ST_VICTORIA;
                    buzzer_play(sfx_victoria, sfx_victoria_d, BUZZER_LEN(sfx_victoria), 120, false);
                }
            }
            break;

        case ST_GAME_OVER:
            img_clear(&g->display->myImage);
            game_drawNumero(&g->display->myImage, g->score, 13);
            // X en la parte superior
            {
                uint8_t i;
                for(i = 0; i < 5; i++){
                    img_setPixel(&g->display->myImage, 6+i, 2+i);
                    img_setPixel(&g->display->myImage, 10-i, 2+i);
                }
            }
            if(botones_flancoDisparo(g->botones)){ game_init(g); }
            break;

        case ST_VICTORIA:
            img_clear(&g->display->myImage);
            game_drawNumero(&g->display->myImage, g->score, 13);
            // "V" de victoria
            {
                uint8_t i;
                for(i = 0; i < 4; i++){
                    img_setPixel(&g->display->myImage, 5+i, 2+i);
                    img_setPixel(&g->display->myImage, 11-i, 2+i);
                }
            }
            if(botones_flancoDisparo(g->botones)){ game_init(g); }
            break;
    }

    // --- Intensidad por sensor de luz ---
    static uint8_t divSensor = 0;
    static uint8_t intensidadPrev = 0xFF;
    if(++divSensor >= 15)
    {
        divSensor = 0;
        uint8_t inten = sensor_luzAIntensidad(ADCLight);
        if(inten != intensidadPrev)
        {
            intensidadPrev = inten;
            display16x32_setIntensity(g->display, inten);
        }
        DL_ADC12_enableConversions(ADC12_0_INST);
        DL_ADC12_startConversion(ADC12_0_INST);
    }

    if(g->estado != estadoPrevSerial)
    {
        estadoPrevSerial = g->estado;
        if(g->estado == ST_JUGANDO)
            Driver_Uart_sendMessageUart(g->uart, "JUGANDO\r\n");
        else if(g->estado == ST_PAUSA)
            Driver_Uart_sendMessageUart(g->uart, "PAUSA\r\n");
    }

    display16x32_sendFrame(g->display);
}
