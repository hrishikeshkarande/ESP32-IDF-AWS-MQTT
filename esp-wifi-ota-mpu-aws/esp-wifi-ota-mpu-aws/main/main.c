/**
 * Application entry point.
 */

#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_log.h" 

#include "wifi_app.h"
#include "mpu6050.h"
#include "sensor_task.h" 
//#include "i2cdev.h" // <--- 1. ENSURE THIS IS INCLUDED (Usually pulled in by mpu6050.h, but safe to add)

// Define the I2C descriptor globally so all tasks can access it
#define I2C_PORT            I2C_NUM_0
#define I2C_SDA_PIN         GPIO_NUM_1
#define I2C_SCL_PIN         GPIO_NUM_2
#define MPU_ADDR            MPU6050_I2C_ADDRESS_LOW // 0x68

// !!! THIS MUST BE DEFINED GLOBALLY !!!
mpu6050_dev_t sensor_dev;

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // --- CRITICAL FIX: Initialize the I2C Library FIRST ---
    ESP_ERROR_CHECK(i2cdev_init()); 
    // ------------------------------------------------------

    // --- 1. MPU-6500 I2C Initialization ---
    ESP_LOGI("MPU", "Initializing MPU6500 driver...");
    ESP_ERROR_CHECK(mpu6050_init_desc(&sensor_dev, MPU_ADDR, I2C_PORT, I2C_SDA_PIN, I2C_SCL_PIN));
    ESP_ERROR_CHECK(mpu6050_init(&sensor_dev));

    // 2. Start the MPU Sensor Task
    sensor_task_start();

    // 3. Start Wifi
    wifi_app_start();
}