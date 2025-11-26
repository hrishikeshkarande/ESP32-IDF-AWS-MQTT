/*
 * sensor_task.h
 *
 * Defines the public function to initialize and start the MPU sensor data reading task.
 */
#ifndef MAIN_SENSOR_TASK_H_
#define MAIN_SENSOR_TASK_H_

/**
 * @brief Starts the MPU sensor reading FreeRTOS task.
 */
void sensor_task_start(void);

#endif /* MAIN_SENSOR_TASK_H_ */