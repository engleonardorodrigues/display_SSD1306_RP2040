/*****************************************************************************************************
 * Título        : Trabalhando com interrupções no Raspberry Pi Pico
 * Desenvolvedor : Leonardo Rodrigues
 * Versão        : 1.0.0
 * 
 * Descrição:
 * Este programa implementa um sistema de controle interativo utilizando o Raspberry Pi Pico. O código
 * permite a manipulação de uma matriz 5x5 de LEDs WS2812, onde os números de 0 a 9 são exibidos. O sistema
 * responde a interrupções geradas por botões, incrementando ou decrementando o número exibido na matriz.
 * Além disso, o LED vermelho pisca a 5 Hz continuamente, sem bloquear a execução do programa.
 * 
 * Materiais utilizados:
 * 
 * 1 - Raspberry Pi Pico
 * 1 - Matriz 5x5 de LEDs WS2812
 * 2 - Botões de pressão
 * 1 - LED RGB (vermelho, verde e azul)
 * 1 - Resistor de 330 ohms
 ******************************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/uart.h"

//arquivo .pio
#include "pio_matrix.pio.h"

#define BOTAO_A 5 
#define BOTAO_B 6

#define LED_VERMELHO 13 
#define LED_VERDE    12 
#define LED_AZUL     11 

#define WS2812_PIN    7

// Definição da I2C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

// Definição da UART
#define UART_ID uart0 
#define BAUD_RATE 115200 
#define UART_TX_PIN 0 
#define UART_RX_PIN 1


volatile int numero_atual = -1;
volatile bool atualizar_display = false;
volatile uint32_t valor_led; 
volatile PIO pio; 
volatile uint sm; 
volatile double r; 
volatile double g; 
volatile double b;

// Blink a 5 Hz → 10 toggles por segundo = 100 ms cada toggle
#define LED_BLINK_INTERVAL_US  100000  

// Armazena o último tempo em que o LED foi toggled
static uint64_t ultimo_toggle_led = 0;

// Armazena o último tempo em que houve pressionamento detectado

#define DEBOUNCE_DELAY_US  100000
static uint64_t ultimo_tempo_a = 0;
static uint64_t ultimo_tempo_b = 0;

volatile bool led_g_status = true;
volatile bool led_b_status = true;

volatile char last_button_press = '\0';

bool cor = true;

/******************************************MOSTRAR NÚMEROS 0 A 9************************************/

int matriz_numeros[10][5][5] =  { 

    // Número 0
    {1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 1.0, 
     1.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0},

    // Número 1
    {1.0, 1.0, 1.0, 1.0, 1.0,
     0.0, 0.0, 1.0, 0.0, 0.0, 
     0.0, 0.0, 1.0, 0.0, 0.0,
     0.0, 1.0, 1.0, 0.0, 0.0,
     0.0, 0.0, 1.0, 0.0, 0.0},

    // Número 2
    {1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 0.0, 
     1.0, 1.0, 1.0, 1.0, 1.0,
     0.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0},

    // Número 3
    {1.0, 1.0, 1.0, 1.0, 1.0,
     0.0, 0.0, 0.0, 0.0, 1.0, 
     1.0, 1.0, 1.0, 1.0, 1.0,
     0.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0},

    // Número 4
    {1.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 1.0, 
     1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 1.0},

    // Número 5
    {1.0, 1.0, 1.0, 1.0, 1.0,
     0.0, 0.0, 0.0, 0.0, 1.0, 
     1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 0.0,
     1.0, 1.0, 1.0, 1.0, 1.0},

    // Número 6
    {1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 1.0, 
     1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 0.0,
     1.0, 1.0, 1.0, 1.0, 1.0},

    // Número 7
    {1.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 1.0, 
     1.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0},

    // Número 8
    {1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 1.0, 
     1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0},

    // Número 9
    {1.0, 1.0, 1.0, 1.0, 1.0,
     0.0, 0.0, 0.0, 0.0, 1.0, 
     1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0},
};

/*************************************ROTINA DE INTERRUPÇÃO DOS BOTÕES************************************/

void botoes_irq_handler(uint gpio, uint32_t events) {
    uint64_t tempo_atual = time_us_64();  

    if (gpio == BOTAO_A && gpio_get(BOTAO_A) == 0) {
        
        if ( (tempo_atual - ultimo_tempo_a) >= DEBOUNCE_DELAY_US ) {
            ultimo_tempo_a = tempo_atual;   // atualiza o tempo
             gpio_put(LED_VERDE,led_g_status);
             led_g_status = !led_g_status;

             last_button_press = 'A';
       
        }
    }
    else if (gpio == BOTAO_B && gpio_get(BOTAO_B) == 0) {
        if ( (tempo_atual - ultimo_tempo_b) >= DEBOUNCE_DELAY_US ) {
            ultimo_tempo_b = tempo_atual;
             gpio_put(LED_AZUL,led_b_status);
             led_b_status = !led_b_status;
                         
            last_button_press = 'B';
        }
    }
}
/********************************ROTINA PARA DESENHAR NA MATRIZ DE LED************************************/

uint32_t matrix_rgb(double b, double r, double g) {
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

void desenho_pio(int *matriz_numeros, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);

void number_animation(uint32_t valor_led, PIO pio, uint sm, double r, double g, double b) {
    int num_matrizes = sizeof(matriz_numeros) / sizeof(matriz_numeros[0]);
    for (int frame = 0; frame < num_matrizes; frame++) {
        desenho_pio((int *)matriz_numeros[frame], valor_led, pio, sm, r, g, b);
    }
}

void desenho_pio(int *matriz_numeros, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b) {
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            int led_index = row * 5 + col;
            valor_led = matrix_rgb(b, matriz_numeros[led_index], g);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    }
}

/********************************ROTINA PARA ESCREVER NO DISPLAY SSD1306************************************/

void display_ss1306(){

}
/*****************************************ROTINA PRINCIPAL********************************************/

int main() {

 /*****************************************CONFIGURAÇÃO DOS LEDS********************************************/    
    stdio_init_all();
    // Inicializa LED vermelho
    gpio_init(LED_VERMELHO);
    gpio_init(LED_VERDE);
    gpio_init(LED_AZUL);

    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_set_dir(LED_AZUL, GPIO_OUT);

    gpio_put(LED_VERMELHO, 0);
    gpio_put(LED_VERDE, 0);
    gpio_put(LED_AZUL, 0);

 /*****************************************CONFIGURAÇÃO DOS BOTÕES********************************************/    

    // Configura Botão A
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &botoes_irq_handler);

    // Configura Botão B
    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &botoes_irq_handler);
    

 /*****************************************CONFIGURAÇÃO DO WS2812********************************************/    

    // Configura PIO para a matriz WS2812
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, WS2812_PIN);
    
    // Ajuste de cor para a matriz
    double r = 1.0, g = 0.0, b = 0.0;
    uint32_t valor_led;

    // Marca o último toggle do LED como o momento atual
    ultimo_toggle_led = time_us_64();

   
 /*****************************************CONFIGURAÇÃO DO DISPLAY SSD1306********************************************/    

  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  
  ssd1306_t ssd; // Inicializa a estrutura do display
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

while (true) {
  
 // Limpa o display UMA única vez
    ssd1306_fill(&ssd, 0);

    // Escolhe qual mensagem desenhar com base no último botão pressionado
    if (last_button_press == 'A') {
        // Se LED verde está ON (ou seja, led_g_status = false, pois você inverteu),
        // escolha a forma de exibir:
        if (!led_g_status) {
            ssd1306_draw_string(&ssd, "Verde ligado", 15, 30);
        } else {
            ssd1306_draw_string(&ssd, "Verde desligado", 3, 30);
        }
    }
    else if (last_button_press == 'B') {
        if (!led_b_status) {
            ssd1306_draw_string(&ssd, "Azul ligado", 15, 30);
        } else {
            ssd1306_draw_string(&ssd, "Azul desligado", 3, 30);
        }
    }
    else {
        // Se nenhum botão foi pressionado ainda, pode deixar vazio ou mostrar algo:
        ssd1306_draw_string(&ssd, "Aguardando...", 10, 30);
    }

    // Atualiza o display
    ssd1306_send_data(&ssd);

        // Rotina para atualizar o display
        if (atualizar_display) {
            desenho_pio((int *)matriz_numeros[numero_atual], valor_led, pio, sm, r, g, b);
            atualizar_display = false;
             
        }
    }
}
