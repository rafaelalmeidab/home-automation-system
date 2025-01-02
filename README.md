# Home Automation System

This repository contains the source code for a Home Automation System developed as part of a final academic project. The system integrates a mobile application with an electronic circuit to manage and monitor various home automation tasks.

## Features

- **Zero-Crossing Detection:** For precise control of AC loads.
- **Arduino OTA:** Over-the-air updates for the ESP32 microcontroller.
- **Relay Control:** Manage appliances and other electronic devices.
- **Infrared Sensors:** Support for remote control functionality.
- **Temperature Monitoring:** Using the DS18B20 sensor.
- **Humidity Monitoring:** Using the DHT11 sensor.
- **Embedded Web Server:** Hosted on the ESP32 for local control and monitoring.

## Technologies Used

- **ESP32 Microcontroller**: Core hardware platform.
- **C/C++ Programming**: Used for embedded system development.
- **Arduino Framework**: Simplifies microcontroller programming.
- **HTTP Server**: For communication with the mobile application.
- **Sensors and Modules**: DS18B20, DHT11, relays, and infrared sensors.

## Getting Started

Follow these steps to set up and run the project on your ESP32:

### Prerequisites

1. **Clone the Repository**:

   ```bash
   git clone https://github.com/rafaelalmeidab/home-automation-system.git
   cd home-automation-system
   ```

2. **Install Arduino IDE**:

   - Download and install the Arduino IDE from [here](https://www.arduino.cc/en/software).

3. **Set Up ESP32 Board**:

   - Follow the official [ESP32 setup guide](https://docs.espressif.com/projects/arduino-esp32/en/latest/) to configure your Arduino IDE.

4. **Install Required Libraries**:
   Open the Arduino IDE and install the following libraries via the Library Manager:
   - `ArduinoOTA`
   - `DHT`
   - `OneWire`

### Configuration

1. **Update Wi-Fi Credentials**:

   - In the source code, locate the section for Wi-Fi credentials and update them with your network details:
     ```c
     const char* ssid = "Your_SSID";
     const char* password = "Your_PASSWORD";
     ```

2. **Modify Pin Configurations**:
   - Adjust the pin assignments in the code to match your hardware setup.

### Upload the Code

1. Connect your ESP32 board to your computer using a USB cable.
2. Select the appropriate board and port in the Arduino IDE.
3. Compile and upload the sketch to the ESP32:
   ```bash
   # In Arduino IDE, click the 'Upload' button.
   ```

### Run the System

1. After successful upload, the ESP32 will restart and connect to the specified Wi-Fi network.
2. Note the IP address displayed in the Serial Monitor.
3. Open your browser and navigate to the ESP32's IP address to access the web interface:
   ```bash
   http://<esp32-ip-address>
   ```

## Usage

- Control relays to turn appliances on/off.
- Monitor temperature and humidity in real-time.
- Use the infrared remote to send commands to the system.

## Projects

### Summary of Functionalities

1. **Relay Control**

   - Manage connected appliances.

2. **Temperature and Humidity Monitoring**

   - Real-time data display via sensors.

3. **Infrared Command Processing**

   - Supports basic remote-control commands.

4. **OTA Updates**
   - Update firmware without reconnecting the device.

### Run Individual Features

1. Clone this repository:

   ```bash
   git clone https://github.com/rafaelalmeidab/home-automation-system.git
   ```

2. Navigate to the project directory:

   ```bash
   cd home-automation-system
   ```

3. Open the `.ino` file in the Arduino IDE.

4. Connect the microcontroller (ESP32) and ensure the correct board and port are selected in the IDE.

5. Upload the code to your microcontroller and observe the results.

6. Additional instructions for individual features can be found in the source code comments.

## Hardware Requirements

- ESP32 Development Board
- DS18B20 Temperature Sensor
- DHT11 Humidity Sensor
- Relay Module(s)
- Infrared Receiver
- Power Supply
- Supporting Electronics (resistors, capacitors, etc.)

## Future Improvements

- Integration with popular voice assistants (e.g., Alexa, Google Assistant).
- Enhanced mobile application with real-time notifications.
- Cloud connectivity for remote access.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

Special thanks to the academic advisors and peers who supported the development of this project.
