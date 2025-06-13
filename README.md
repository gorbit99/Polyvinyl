
- Sensors
    - [ ] Defining sensors
    - [X] Reading sensor output
    - [X] Fusing sensor output
    - [ ] Handling self-fusing sensors
    - [ ] Sensor config
- Networking
    - [X] Connecting to Wi-Fi
    - [X] Finding server
    - [X] Sending sensor output to server
    - [ ] Bundles
    - [ ] Feature flags
- Interfaces
    - [X] I2C
    - [ ] SPI
- Peripherals
    - [ ] Reading battery and sending it to the server
- [ ] Serial commands
- [ ] LED

IMUs (only that make sense for now)

- [ ] ICM45686 (Missing clock support)
- [ ] ICM45605
- [ ] BNO08X
- [ ] LSM6DSR
- [ ] LSM6DSO
- [ ] LSM6DSV
- [ ] BMI270

Later:

- ESP-Now
- OTA
- Mag support
- Flex sensor support

Not planned:

- Separate I2C buses (instead possibly I2C Mux)
- Sensor calibration (for now at the very least)
