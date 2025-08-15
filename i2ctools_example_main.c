#include <stdio.h>
#include "esp_err.h"
#include "driver/i2c.h"
#include "driver/i2c_master.h"

//Definição de pinos 
#define sda_port  21
#define scl_port  22

//Definição das características do mestre
#define pullup 1
#define talk_freq 100000
#define master_port 0
#define buffer_tx 0
#define buffer_rx 0

//Definição das características do escravo
#define address_dev 0x5A
#define   addres_reg 0x07    // Registrador que armazena a temperatura objeto

//Configuração do mestre do barramento (este esp32) (pronto pra se comunicar após ela)

void i2c_master_config_init(){
    i2c_config_t setup = {
         .mode = I2C_MODE_MASTER,
         .sda_io_num = sda_port,
         .scl_io_num = scl_port,
         .sda_pullup_en = GPIO_PULLUP_ENABLE,
         .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = talk_freq,
        .clk_flags = 0,  
    };

    i2c_param_config(master_port,&setup);

    i2c_driver_install(master_port,setup.mode,buffer_rx,buffer_tx,0);
}


//Se comunicando
float i2c_request_tempC(){
    uint8_t reg=addres_reg;
    uint16_t temp_bruta;
    float temp_celsius;
    uint8_t data[3]; //2 bytes de dados e 1 de crc (sensor tem conversor ADC de 17bits!)
     esp_err_t reading = i2c_master_write_read_device( //Read write chama sensor e pega dados instantâneamente
        master_port, //Porta do mestre
        address_dev, //Endereço do dispositivo
        &reg, //Endereço do registrador
        sizeof(reg),
        data, //Armazenamento de dados lidos
        3, //Numero de bytes lidos
        1000/portTICK_PERIOD_MS); //Frequência de leitura

        if(reading != ESP_OK){
            return 10000.00;
        }
    
        temp_bruta = (data[1]<<8)| data[0];
        temp_celsius = temp_bruta*0.02 -273.15; //Advinda do data sheet
        return temp_celsius;
}

void app_main(void)
{
    float temperature;
    i2c_master_config_init();

    while(1){
        temperature=i2c_request_tempC();
        if(temperature==10000){
            printf("Unsuccessful read");
        }else{
            printf("\n Temperature read: %.2f \n",temperature);
        }
        vTaskDelay(500/portTICK_PERIOD_MS);

    }
}

