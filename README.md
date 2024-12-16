# Development of Image Accelerator and Interfacing with Linux Kernel Module in a Custom Yocto Image

<p align="center">
  <img src="./QEMU-Logo.wine.png" width="200" style="display: inline-block; margin-right: 10px;">
  <img src="./Yocto-Linux.png" width="200" style="display: inline-block;">
  <img src="./polito.jpg" width="200" style="display: inline-block;">
</p>



## Project Overview
This project involves developing a custom hardware image accelerator and integrating it into a QEMU virtual board environment. The hardware will be interfaced with a Linux Kernel Module (LKM) and run within
 a custom-built image generated using the Yocto Project.

## Objective
The goal is to create a seamless pipeline where:

- The userspace application interacts with the virtual hardware.
- Data is sent from the userspace application to the image accelerator for processing.
- The processed image data is sent back to the userspace application for display or further use.

## Key Components
- Custom Hardware: A virtual hardware accelerator for image processing.
- QEMU Virtual Environment: Simulated hardware environment for development and testing.
- Linux Kernel Module (LKM): Serves as the interface between the custom hardware and the Linux operating system.
- Yocto Project: Used to build a minimal and tailored Linux image containing the necessary drivers and applications.
- Userspace Application: Acts as the front-end for sending image data to the hardware and displaying the processed output.

## Workflow

- Before diving into our workflow, it’s essential to set up the working environment to ensure a seamless and efficient process without any disruptions. The two primary technologies we will be utilizing are:

- Yocto Project
- QEMU

 But Before starting our development process we will first make a seprate directory where we can organise our entire development procedure.

      ```bash
      mkdir project
      cd project

  ## Yocto Project
  - The Yocto Project is designed to enable the development of a Linux OS customized for specific embedded devices.
    It provides powerful tools that not only allow us to tailor Linux to meet our specific requirements but also support
    and streamline the entire development process.

    ### Setting up Yocto Project 
       The Yocto Project offers a wide range of capabilities, which are thoroughly documented in the Yocto Project manual. 
       The manual provides comprehensive and well-structured guidance. However, our focus here will
       be limited to aspects specifically related to our project.
      ```bash
      git clone https://github.com/yoctoproject/poky 
##  QEMU 
  - The main idea behind QEMU is to emulate virtual hardware for various architectures (e.g., ARM, RISC-V) different from the host architecture.
    For example, you can emulate an ARM processor along with its entire board on an x86 architecture. The advantage of using QEMU is that it enables quick prototyping of hardware behavior,
    reducing development and testing time. Additionally, it allows us to test our Linux OS on the hardware emulated by QEMU. 
    QEMU supports a wide range of architectures and boards, including Virt, Raspberry Pi, STM32, and many others.

###  Setting up QEMU
        For setting up the enviroment of QEMU we will follow the folowing sequence of commands
        ```bash

        
        The board that we are focusing on is 'virt' which is a board that is best for prototyping and supports ARM processor and also we can connect
        several types of devices
        Board:'virt'
        CPU
