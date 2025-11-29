#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_timer.h"
#include "rom/ets_sys.h"
#include "rom/ets_sys.h"
#include "driver/gpio.h"

#include "dht11.h"

static gpio_num_t dht_gpio;

void DHT11_init(gpio_num_t gpio_num) {
    dht_gpio = gpio_num;
    // Pequeno delay para estabilizar
    vTaskDelay(200);
}

// Função auxiliar para esperar o pino mudar de estado
// Retorna o tempo que levou em microssegundos
static int _wait_pulse(int state, int timeout_us) {
    int count = 0;
    while (gpio_get_level(dht_gpio) == state) {
        if (count >= timeout_us) return -1; // Timeout
        ets_delay_us(1); // Espera 1 microsegundo
        count++;
    }
    return count;
}

dht11_reading_t DHT11_read(void) {
    dht11_reading_t result = {0, 0, -1};
    uint8_t data[5] = {0,0,0,0,0};

    // 1. Start Signal
    gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(dht_gpio, 0);
    ets_delay_us(20000); // 20ms em Low
    gpio_set_level(dht_gpio, 1);
    ets_delay_us(40);
    gpio_set_direction(dht_gpio, GPIO_MODE_INPUT);

    // 2. Esperar resposta do sensor (80us Low, 80us High)
    if (_wait_pulse(0, 85) == -1) { result.status = -2; return result; }
    if (_wait_pulse(1, 85) == -1) { result.status = -2; return result; }

    // 3. Ler 40 bits
    for (int i = 0; i < 40; i++) {

        if (_wait_pulse(0, 55) == -1) { result.status = -2; return result; }
        
        // Mede a duração do High para saber se é bit 0 ou 1
        int duration = _wait_pulse(1, 75);
        if (duration == -1) { result.status = -2; return result; }

        // Se durou mais que 28us, é bit 1. (Bit 0 dura ~28us)
        if (duration > 28) {
            // preenche o array
            data[i / 8] |= (1 << (7 - (i % 8)));
        }
    }

    // 4. Verificar Checksum
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        result.humidity = data[0];
        result.temperature = data[2];
        result.status = 0; // Sucesso
    } else {
        result.status = -1; // Erro de Checksum
    }

    return result;
}