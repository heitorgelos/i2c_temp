#include "driver/i2c.h"
#include <stdio.h>

#define I2C_MASTER_NUM          I2C_NUM_0
#define I2C_MASTER_SDA_IO       21
#define I2C_MASTER_SCL_IO       22
#define I2C_MASTER_FREQ_HZ      100000

#define MLX90614_ADDR           0x5A    // Endereço padrão I2C do sensor
#define MLX90614_REG_OBJ_TEMP   0x07    // Registrador temperatura objeto

void i2c_master_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

float mlx90614_read_temperature(void) {
    uint8_t reg = MLX90614_REG_OBJ_TEMP;
    uint8_t data[3]; // 2 bytes dados + 1 byte CRC (ignoraremos CRC aqui)
    
    esp_err_t ret = i2c_master_write_read_device(
        I2C_MASTER_NUM,
        MLX90614_ADDR,
        &reg, sizeof(reg),
        data, 3,
        1000 / portTICK_PERIOD_MS
    );

    if (ret != ESP_OK) {
        printf("Erro na leitura I2C: %d\n", ret);
        return -1000.0f; // valor inválido para indicar erro
    }

    uint16_t raw_temp = (data[1] << 8) | data[0];
    float temp_celsius = (raw_temp * 0.02f) - 273.15f;
    return temp_celsius;
}

void app_main(void) {
    i2c_master_init();
    while (1) {
        float temp = mlx90614_read_temperature();
        if (temp > -1000) {
            printf("Temperatura objeto: %.2f°C\n", temp);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
