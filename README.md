# ESP32-S3 IoT Motion Node (Wi-Fi, OTA, MPU-6500)

This project implements a complete, highly concurrent IoT node using the **ESP32-S3** microcontroller and the robust **ESP-IDF (v5.5)** framework. It demonstrates professional-grade embedded development practices by integrating a real-time motion sensor (**MPU-6500**) with a local HTTP server dedicated to Wi-Fi provisioning and live data monitoring. The architecture is built on **FreeRTOS** for reliable multitasking.

🚀 **Key Features**

**MPU-6500 Sensor Interface:** Reads 6-axis acceleration, rotation, and temperature data using the I2C protocol. The MPU driver is integrated as a clean C component, providing reliable data acquisition in standard units (g, degrees/s, Celsius). The robust communication flow ensures thread-safe access to sensor data across the application cores.

**Concurrency (FreeRTOS):** Utilizes separate dedicated FreeRTOS tasks for managing hardware and network logic, eliminating blocking issues. For example, the sensor reading task runs on a dedicated core, preventing high-frequency I2C operations from interfering with the network stack. This separation ensures stability and responsiveness.

**Web Provisioning:** Implements the modern SoftAP + Station workflow. The ESP32 first boots, hosting a temporary Wi-Fi Access Point (`ESP32_AP`), allowing any client device (phone/PC) to connect. The client then inputs the desired home router credentials via a webpage, which are passed back to the ESP32 to configure its Station mode connection.

**Real-time Dashboard:** A locally hosted HTTP server serves an AJAX-polling webpage (`/mpuSensor.json`). This frontend displays the live motion data (Accel X/Y/Z, Gyro X/Y/Z, Temp) updating every second without requiring a page refresh, offering immediate visual feedback on the device state.

**Over-the-Air (OTA) Updates:** Provides a critical production feature: the ability to upload a new firmware `.bin` file directly through the web interface. This allows for wireless updates, simplifying deployment and maintenance by eliminating the need for physical USB access after the initial setup.

**Status Indication:** An RGB LED, controlled via the high-speed LEDC peripheral, provides crucial feedback on network status to the user (e.g., pulsing yellow for AP running, solid green for Wi-Fi connected).

---

🛠️ **Hardware & Pinout Configuration**

**Target Hardware**

- Microcontroller: ESP32-S3-DevKitC-1  
- Sensor: MPU-6500 (or MPU-6050)  
- Indicators: External RGB LED (or 3 separate R, G, B LEDs)

🔌 **Pinout**

We explicitly selected **GPIO 1** and **GPIO 2** for I2C communication to ensure stability on the ESP32-S3, as these pins are less prone to conflicts with the native USB/JTAG interface compared to GPIOs like 19 and 20.

| Component | Pin Name | ESP32-S3 GPIO | Note |
|----------|----------|---------------|------|
| MPU-6500 | SDA      | GPIO 1        | I2C Data Line — **requires 4.7kΩ pull-up to 3.3V** |
|          | SCL      | GPIO 2        | I2C Clock Line — **requires 4.7kΩ pull-up to 3.3V** |
|          | VCC      | 3.3V          | |
|          | GND      | GND           | |
| RGB LED  | Red      | GPIO 21       | LEDC Channel 0 (PWM Output) |
|          | Green    | GPIO 22       | LEDC Channel 1 (PWM Output) |
|          | Blue     | GPIO 23       | LEDC Channel 2 (PWM Output) |

⚠️ **I2C Protocol Requirement:**  
The use of **4.7kΩ pull-up resistors** on both SDA and SCL lines (to 3.3V) is mandatory. Without these physical pull-ups, the I2C open-drain lines cannot transition to a HIGH state correctly, causing bus initialization failure and triggering a software panic. This was a significant debug point.

---

⚙️ **Installation & Build Guide**

### Prerequisites

- VS Code with Espressif ESP-IDF Extension installed  
- ESP-IDF Framework (v5.5.1)

### Build Procedure

**1. Clone the Repository:**

    git clone <your-repo-url>
    cd esp-wifi-ota-mpu-aws

**2. Install Component Dependencies:**

The project relies on community components (`i2cdev`, `esp_idf_lib_helpers`). This command uses the component registry to download and resolve external library paths automatically:

    idf.py install

**3. Configure Project Settings (`idf.py menuconfig`):**

These settings must be manually enabled in your environment:

- **Core Driver:**  
  Navigate to:  
  `Component config -> Driver configurations -> Enable I2C`  
  (This enables the necessary hardware peripheral access and mutex creation.)

- **Partition Table (CRITICAL FIX):**  
  To accommodate the large Wi-Fi/HTTP/MPU binary (which exceeds 1MB), the partition size must be explicitly configured:  

  Navigate to:  
  `Partition Table -> Select "Single factory app, no OTA"`  
  (This allocates the necessary 2MB or more space to the main application partition.)

**4. Build and Flash:**

The build process now successfully links all files (`.c` files for MPU, Wi-Fi, HTTP) and bundles the web assets (`webpage/`):

    idf.py build
    idf.py flash monitor

---

🐛 **Key Troubleshooting Fixes (Resolved Issues in the Codebase)**

**I2C Initialization Panic (`xQueueSemaphoreTake`):**  
This runtime crash was solved by ensuring the required low-level library setup was executed before any device communication: calling `i2cdev_init()` in `main.c` immediately before initializing the MPU driver.

**Linker Errors (`undefined reference to 'sensor_dev'`):**  
This common C issue was resolved by correctly defining the MPU device descriptor (`mpu6050_dev_t sensor_dev;`) in the global scope of `main.c`, and crucially, avoiding the `static` keyword, which would have restricted its visibility to the linker.

**Missing Headers:**  
The comprehensive `REQUIRES` list in `main/CMakeLists.txt` and explicit inclusion of headers like `esp_log.h` addressed numerous compiler failures related to core component dependencies.

---

🔮 **Future Development**

**AWS Integration:**  
The next major step is to implement the MQTT communication task using `esp-mqtt`. This task will read the live motion data from the `sensor_task` and publish it to an AWS IoT Core endpoint, completing the cloud pipeline.

**NVS Persistence:**  
Currently, Wi-Fi credentials are lost on reboot. Future work should focus on saving provisioned Wi-Fi configuration (SSID/Password) to the NVS flash partition so the device automatically reconnects to the router.

**Web UI Improvement:**  
Enhance the dashboard by adding visual elements, such as real-time line charts (using Chart.js), to plot the X, Y, and Z acceleration trends over time, offering better data analysis.

---

This project demonstrates a full, modern IoT workflow: real-time sensing, concurrent FreeRTOS task separation, network provisioning, OTA updates, and a dynamic web dashboard — all running on the ESP32-S3.
