# 6-DOF Robotic Arm Project

## Introduction
This project involves developing a 6-degree-of-freedom (DOF) robotic arm controlled by an STM32F103C8T6 microcontroller. The system uses `RTOS` for task management and `OpenCV` with ArUco markers for image processing. Communication between the robotic arm and the computer is achieved via the HC-06 Bluetooth module using USART and the `socket` library in Python.

## Features
- Control of a 6-DOF robotic arm.
- Task management using RTOS.
- Image processing with OpenCV and ArUco markers.
- Wireless communication via the HC-06 Bluetooth module using the `socket` library.
- PWM signal generation using timers in STM32 to control servos.

## System Requirements
- STM32F103C8T6 microcontroller.
- HC-06 Bluetooth module.
- Computer with Python and OpenCV installed.
- Additional electronic components such as servo motors, power supply, etc.
