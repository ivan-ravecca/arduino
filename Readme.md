# Arduino Projects

This Arduino project monitors two AC input lines and reports their status (energized/de-energized) over the internet. It includes audio alerts for status changes.

## Getting Started

### Prerequisites

- Arduino IDE installed
- Arduino board (Uno recommended)
- USB cable for programming and debugging

### Required Components

- Arduino UNO microcontroller
- 2x ZMPT101B voltage sensor modules
- Active buzzer module
- Ethernet shield with W5100 chipset

### Installation

1. Clone or download this repository
2. Open the Arduino IDE
3. Load the project file (.ino)
4. Select your Arduino board type and COM port
5. Compile and upload the code to your board

## Project Structure

```
/
├── Reading/
│   ├── ac_monitor.ino
│   ├── config.h
│   └── README.md
└── docs/
    └── wiring_diagram.png TBD
```

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/improvement`)
3. Commit your changes (`git commit -am 'Add new feature'`)
4. Push to the branch (`git push origin feature/improvement`)
5. Open a Pull Request with detailed description

## Useful Resources

For additional information and troubleshooting, check out these helpful sites:

- [Arduino Official Documentation](https://www.arduino.cc/reference/en/)
- [ZMPT101B Sensor Datasheet and Tutorials](https://components101.com/sensors/zmpt101b-voltage-sensor)
- [Ethernet Shield W5100 Guide](https://www.arduino.cc/en/Guide/ArduinoEthernetShield)
- [Arduino Community Forum](https://forum.arduino.cc/)
- [Circuit troubleshooting guides](https://www.instructables.com/circuits/)
- [Wokwi](https://wokwi.com/)
