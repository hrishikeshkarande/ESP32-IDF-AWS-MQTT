#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

#include "sensor_task.h"
#include "tasks_common.h"
#include "mpu6050.h"

static const char *TAG = "SENSOR_TASK";

// -----------------------------------------------------------------
// External Declarations (Required to access the sensor from main.c)
// -----------------------------------------------------------------
extern mpu6050_dev_t sensor_dev; // Must match the name in main.c
// -----------------------------------------------------------------


/**
 * @brief Main task function for reading MPU data.
 */
static void sensor_task(void *pvParameters)
{
    // Variables to store sensor data
    mpu6050_acceleration_t accel;
    mpu6050_rotation_t gyro;
    float temp;

    ESP_LOGI(TAG, "Starting MPU Sensor Reading Loop on Core %d", xPortGetCoreID());

    for (;;)
    {
        // Attempt to read all 7 values (Accel X/Y/Z, Temp, Gyro X/Y/Z)
        if (mpu6050_get_motion(&sensor_dev, &accel, &gyro) == ESP_OK)
        {
            // Successfully read acceleration and rotation (6-axis motion)
            mpu6050_get_temperature(&sensor_dev, &temp);

            // --- LOGS COMMENTED OUT FOR WEBPAGE VIEWING ---
            // ESP_LOGI(TAG, "--- Motion Data ---");
            // ESP_LOGI(TAG, "Temp: %.2f C", temp);
            // ESP_LOGI(TAG, "Accel: X=%.2f g | Y=%.2f g | Z=%.2f g", accel.x, accel.y, accel.z);
            // ESP_LOGI(TAG, "Gyro:  X=%.2f °/s | Y=%.2f °/s | Z=%.2f °/s", gyro.x, gyro.y, gyro.z);
            // ----------------------------------------------
        }
        else
        {
            ESP_LOGE(TAG, "Failed to read MPU data!");
        }

        // Delay before the next reading (e.g., 500ms update rate)
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief Starts the MPU sensor reading FreeRTOS task.
 */
void sensor_task_start(void)
{
    xTaskCreatePinnedToCore(&sensor_task, 
                            "sensor_task", 
                            SENSOR_TASK_STACK_SIZE, 
                            NULL, 
                            SENSOR_TASK_PRIORITY, 
                            NULL, 
                            SENSOR_TASK_CORE_ID);
}