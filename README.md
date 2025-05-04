# HackUPC 2025 Project

## Project Overview
Our project initially aimed to implement real-time image processing using hardware acceleration. However, due to early challenges with missing hardware components, we had to adapt by using UART communication, which introduced performance limitations. This prevented us from achieving the real-time performance we envisioned.

Midway through the hackathon, we pivoted to benchmarking a 2D convolution algorithm. The focus shifted to comparing the performance of a software-based implementation on the PYNQ PS (Processing System) with a hardware-accelerated version. While we successfully implemented the software version, time constraints prevented us from fully validating the hardware acceleration.

## Repository Highlights
- **`accel_trial` branch**: Contains an early version of a simple hardware accelerator. A testbench in Vivado was implemented to verify basic functionality, but full integration and validation in a hardware-software co-design context were not achieved.
- **`one_image_convolution` folder**: Includes scripts for sending and receiving images over UART and applying a 2D convolution algorithm.

## Lessons Learned
While we didn’t achieve our final goal, the project provided valuable insights into hardware-software co-design and performance benchmarking. Despite the challenges, we are proud of the progress made during this experience.

For more details, visit our [Devpost project page](https://devpost.com/software/convolutional-trials-on-fpga-with-pynq-z2-board).