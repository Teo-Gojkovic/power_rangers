# Team School Project: Sensor Communication with Server

## Project Overview

This project is a collaborative effort by our team as part of our school curriculum. The goal is to design and implement a system where a sensor communicates with a server, and the server stores the received data into a CSV file for further analysis.

## Objectives

1. **Sensor Communication**: Establish a reliable communication channel between the sensor and the server.
2. **Data Storage**: Implement functionality on the server to store incoming sensor data into a CSV file.
3. **Team Collaboration**: Work together as a team to design, develop, and test the system.

## Project Components

### Sensor : Captures data and sends it to the server.

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
![Visual Studio Code](https://img.shields.io/badge/Visual%20Studio%20Code-0078d7.svg?style=for-the-badge&logo=visual-studio-code&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)

### Server :

[![GitHub](https://img.shields.io/badge/GitHub-222222?style=for-the-badge&logo=github&logoColor=white)](https://github.com/)
![Visual Studio Code](https://img.shields.io/badge/Visual%20Studio%20Code-0078d7.svg?style=for-the-badge&logo=visual-studio-code&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)

### CSV Storage : Stores the processed data in a structured CSV format.

![Microsoft Excel](https://img.shields.io/badge/Microsoft_Excel-217346?style=for-the-badge&logo=microsoft-excel&logoColor=white)
![Visual Studio Code](https://img.shields.io/badge/Visual%20Studio%20Code-0078d7.svg?style=for-the-badge&logo=visual-studio-code&logoColor=white)

## Key Insights from the Slideshow

### Communication Protocol

- The system uses a **serial communication protocol** to transfer data from the Arduino-based sensor to the server.
- Data packets are structured to ensure reliability and accuracy during transmission.

### Sensor Details

- The project utilizes a **DHT11 sensor** for temperature and humidity measurements.
- The sensor is connected to an Arduino board, which processes and sends the data to the server.

### Server Implementation

- The server is implemented in **C** and is responsible for:
- Receiving data from the sensor.
- Parsing and validating the incoming data.
- Storing the data in a **CSV file** for further analysis.

### Challenges Addressed

- Ensuring synchronization between the sensor and server.
- Handling data loss or corruption during transmission.
- Designing a scalable and maintainable system architecture.

## Team Roles

### 👨‍💼Team Lead <br>

[![Teo GOJKOVIC](https://img.shields.io/badge/Teo_GOJKOVIC-222e45?style=for-the-badge&logo=github&logoColor=white)](https://github.com/Teo-Gojkovic)

### 👨‍💻 Backend Developer <br>

[![MDulche](https://img.shields.io/badge/MDulche-222e45?style=for-the-badge&logo=github&logoColor=white)](https://github.com/MDulche)

### 📊 Data Analyst <br>

[![ElineBRD](https://img.shields.io/badge/ElineBRD-222e45?style=for-the-badge&logo=github&logoColor=white)](https://github.com/ElineBRD)
[![pascalmaxime](https://img.shields.io/badge/pascalmaxime-222e45?style=for-the-badge&logo=github&logoColor=white)](https://github.com/pascalmaxime)

### 🔧 Sensor Integration Specialists <br>

[![SarahGarf](https://img.shields.io/badge/SarahGarf-222e45?style=for-the-badge&logo=github&logoColor=white)](https://github.com/SarahGarf)

## Project Structure

```
power_rangers
├── archives
│   ├── archives
│   ├── archives.c
│   └── donnees_temp.csv
├── arduino
│   ├── arduino.c
│   └── DHT11_reader.ino
├── client
│   ├── client
│   └── client.c
├── README.md
├── server
│   ├── archives.c
│   ├── archives.h
│   ├── donnees_temp.csv
│   ├── Makefile
│   ├── server
│   └── server.c
└── slideshow
    └── TP_COMMUNICATION_ARDUINO.pdf
```

> The `tree` command was used to create this file structure diagram.

## Future Improvements

- Add real-time data visualization.
- Implement advanced data analysis features.
- Enhance system scalability.

## Contributers

[![SarahGarf](https://img.shields.io/badge/SarahGarf-222e45?style=for-the-badge&logo=github&logoColor=white)](https://github.com/SarahGarf)
[![ElineBRD](https://img.shields.io/badge/ElineBRD-222e45?style=for-the-badge&logo=github&logoColor=white)](https://github.com/ElineBRD)
[![pascalmaxime](https://img.shields.io/badge/pascalmaxime-222e45?style=for-the-badge&logo=github&logoColor=white)](https://github.com/pascalmaxime)
[![MDulche](https://img.shields.io/badge/MDulche-222e45?style=for-the-badge&logo=github&logoColor=white)](https://github.com/MDulche)
[![Teo GOJKOVIC](https://img.shields.io/badge/Teo_GOJKOVIC-222e45?style=for-the-badge&logo=github&logoColor=white)](https://github.com/Teo-Gojkovic)
