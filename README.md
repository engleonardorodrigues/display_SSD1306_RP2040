# Trabalhando com I2C e Display SSD1306 no Raspberry Pi Pico

## Descrição
Este projeto implementa um sistema de controle interativo utilizando o Raspberry Pi Pico. O código permite a manipulação de uma matriz 5x5 de LEDs WS2812 para exibição de números de 0 a 9. Além disso, botões físicos são usados para alterar os números exibidos e um display SSD1306 recebe mensagens via UART.

## Funcionalidades
- Exibição de números de 0 a 9 em uma matriz de LEDs WS2812.
- Uso de interrupções para detectar o pressionamento de botões.
- Atualização do display SSD1306 via I2C para exibir mensagens.
- Comunicação via UART para envio de mensagens ao display.
- Controle de LEDs RGB individuais.

## Materiais Utilizados
- 1 x Raspberry Pi Pico
- 1 x Matriz 5x5 de LEDs WS2812
- 1 x Display SSD1306
- 2 x Botões de pressão
- 1 x LED RGB (vermelho, verde e azul)
- 1 x Resistor de 330 ohms

## Conexões
### LED e Botões
| Componente      | GPIO |
|---------------|------|
| Botão A       | 5    |
| Botão B       | 6    |
| LED Vermelho  | 13   |
| LED Verde     | 12   |
| LED Azul      | 11   |
| Matriz WS2812 | 7    |

### Comunicação I2C (Display SSD1306)
| Componente | GPIO |
|------------|------|
| SDA        | 14   |
| SCL        | 15   |

### Comunicação UART
| Componente | GPIO |
|------------|------|
| TX         | 0    |
| RX         | 1    |

## Estrutura do Código
- **Interrupções:** Captura do pressionamento dos botões para alterar o número exibido e atualizar o display.
- **Controle dos LEDs WS2812:** Função `desenho_pio()` que altera os LEDs para exibir os números.
- **Atualização do Display SSD1306:** Função `ssd1306_draw_string()` para exibir mensagens baseadas nas interações do usuário.
- **Comunicação UART:** Mensagens enviadas para o terminal via UART.

## Instalação e Execução
### Dependências
Certifique-se de que o Raspberry Pi Pico está configurado corretamente com as bibliotecas do SDK do Pico. As bibliotecas necessárias incluem:
- `pico/stdlib.h`
- `hardware/irq.h`
- `hardware/clocks.h`
- `hardware/i2c.h`
- `hardware/pio.h`
- `hardware/uart.h`
- `ssd1306.h` e `font.h` (para controle do display)

## Como Usar
- Pressione o **Botão A** para ativar o LED verde e exibir a mensagem correspondente no display.
- Pressione o **Botão B** para ativar o LED azul e exibir a mensagem correspondente no display.
- O número exibido na matriz de LEDs pode ser incrementado ou decrementado via botões.
- As mensagens no display são atualizadas conforme a interação do usuário.

## 🖥 Simulação no Wokwi (Vídeo Demonstrativo)
Este projeto pode ser testado no simulador online **Wokwi**. Basta clicar no link a seguir para assistir ao vídeo demonstrativo.

## 📩 Contato
Caso tenha dúvidas ou sugestões, entre em contato:
- **Desenvolvedor**: Leonardo Rodrigues
- **Linkedin**: https://www.linkedin.com/in/leonardorodrigues7/
