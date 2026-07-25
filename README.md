# STM32 GPS NTP Server

NTP server based on STM32F411 and W5500 Ethernet controller.

The device receives UTC time from a GPS module via UART and provides time synchronization for NTP clients over Ethernet.

## Overview

This project implements a standalone NTP server running on an STM32 microcontroller.

The STM32 receives time information from a GPS module using NMEA RMC messages. The received date and time are converted into NTP timestamp format and used to answer client requests.

The device does not use an external RTC. After receiving a valid GPS timestamp, the current time is maintained using a hardware microsecond timer.

## Hardware

- STM32F411 microcontroller
- W5500 Ethernet controller
- GPS module with NMEA output


## Software

Developed using:

- STM32CubeIDE
- STM32 HAL
- Wiznet W5500 Ethernet library

## How it works

1. GPS module sends NMEA data over UART.
2. STM32 parses RMC messages and extracts:
   - year
   - month
   - day
   - hour
   - minute
   - second
3. Date and time are converted to NTP timestamp format.
4. STM32 starts answering NTP requests on UDP port 123. 
 

