#include <stdio.h>
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_mac.h"

#include "dht11.h"

#define DHT11_PIN GPIO_NUM_18

void app_main(void)
{
    // Inicializa o pino
    DHT11_init(DHT11_PIN);

    printf("Iniciando leitura do DHT11...\n");

    while(1) {

        dht11_reading_t leitura = DHT11_read();
        
        if (leitura.status == 0) {
            printf("Temperatura: %d C  |  Umidade: %d %%\n", 
                   leitura.temperature, 
                   leitura.humidity);
        } else {
            printf("Erro na leitura! Status: %d\n", leitura.status);
        }

        vTaskDelay(200);
    }
}