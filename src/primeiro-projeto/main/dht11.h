#ifndef DHT11_H
#define DHT11_H

#include "driver/gpio.h"

// Definição da estrutura de leitura
typedef struct dht11_reading{
    int temperature;
    int humidity;
    int status; // 0 = OK, -1 = Erro CRC, -2 = Timeout
} dht11_reading_t;

// Funções públicas
void DHT11_init(gpio_num_t gpio_num);
dht11_reading_t DHT11_read(void);

#endif