# Development of of custome Linux Kernel Module for a custom hardware model

<p align="center">
  <img src="/images/QEMU-Logo.wine.png" width="200" style="display: inline-block; margin-right: 10px;">
  <img src="/images/Yocto-Linux.png" width="200" style="display: inline-block;">
  <img src="/images/polito.jpg" width="200" style="display: inline-block;">
</p>



## Project Overview
The main goal is to develop a Custom Linux Kernel Module for a Custom hardware model .But the flow is not as structured and there are alot of tutorials online
some of them,completly wrong,confusing or incomplete.We don't brag to be the best but atleast we will try to be as clear as possible.

## What is Linux Kernel Module(LKM)?
A **Linux Kernel Module (LKM)** is a piece of software that helps in extending the ""Linux kernel's functionality** without modifying the kernel itself the modular approach helps the kernal to add and remove the fetures dynamicaly.

### Why use LKM's?

- They allow to extend the kernel functionality without recompiling the entire kernel
- They enable modularity means that we can **Load/unload drivers dynamicaly**.
- They provide critical functionalities like **device drivers, network drivers,a nd system call extensions**.

  
Since our focus is **device drivers**, we will mainly discuss:

- **Interfacing with hardware**
- **Acessing memory regions**
- **Communicating with user-space applications**

---

## Project Components

This project is divided in to two major parts:
### 1. Hardware
- We wil design a model of accelarator and simulate it using QEMU
- We will simualte an already avaliable board available in a large repository of QEMU hardware
   emulation which will act as an main computing platform.

### 2. software
- We will be developing an entire Linux Distribution from the scratch using Yocto project which will 
  be tailored to only our hardware requirements
- We will develop Linux Kernel Module for our accelarator
- We will write a userspace application for using our hardware via Linux Kernel Module(LKM)

---
## Tools
The software tools that we are using to realize our goals are:

- Yocto Project
- QEMU

  these two technologies have a learning curve but worth it.As this tutorial donot cover the 
  lerning and setting up these two technologies ,i will provide some the refrences at the end of
  the tutorial so that anyone who want's to follow has a starting point.
---

## Hardware
 The hardware implementations has three dsitinct steps:
 - Write a device model which compliant with QEMU subsytem
 - Write a Bash script that will not only include that new device model but also the entire environment of our computing platform

### Accelarator
#### Idea
Our hardware is designed to perform convoloution operation on matrices:
- An 8x8 input matrix (which is reresenting a image or any value)
- An 3x3 kernel matrix(which is representing a filter)
The accelarator applies convoloution operation by slinding kernel matrix over the input matrix,performing
element wise multiplication,and summing the results.Thsi computation produces a 6x6 output matrix,where each element in the 
output matrix representing the convoloution sum of a 3x3 region of the input matrix.
The process of convoloution is normaly used in iamge processing,edge detection and feature extraction,making the accelarator suitable
for high speed matri computation
<p align="center">
  <img src="/images/2dconv-84a92b2e7cce6f31ad9fba1e57841198.gif" width="200" style="display: inline-block;">
</p>

#### Features
- 128 bytes of memory
- Supports unsigned 8bit data type only
- 8bit CSR(Control and Status Register)
- PCI communication interface

#### Archietecture Overview
Accelarator archietecture consist of:
- Convoloution Logic CORE performs convoloution opearation
- Hardware internal Registers
##### Memory Layout
Total memory footprint is about 128 bytes.The internal memory is divided into 5 regions:
- Kernel:3x3=9bytes
- Image:8x8=64bytes
- Result:6x6=36bytes
- CSR:1byte
- Reserved: 18bytes

<p align="center">
  <img src="/images/Screenshot from 2025-02-17 18-22-08.png" width="100" style="display: inline-block;">
  <img src="/images/Screenshot from 2025-02-17 18-22-40.png" width="180" style="display: inline-block;">

</p>

---






Linux Kernel Module(LKM) is a piece of software which is helping the kernel to extend it's functionality,adn can make kernel functionality modular.
The LKM can provide lot's of capabilites such as: network driver,device drivers etc.We will focus on Device Drivers as they are our main interest right now.
Inorder to achieve our goal we have to make some arrangments,such as LKM can not run alone as we discussed earliar indeed they are  software component 
but they our run and managed by a Linux Operating System,Also we need a user space application to access that LKM.Device Driver provides a file like interface
of the memory regions of the device,Hence we need a device too.

Our project is divided into two major components:

-hardware:We are modeling a hardware and simulating that hardware with an cortex a-15 32bit processor using QEMU
-software 
let's discuss each of these part's breifly

##Hardware





## Objective
The goal is to create a seamless pipeline where:

- The userspace application interacts with the virtual hardware.
- Data is sent from the userspace application to the accelerator for processing.
- The processed  data is sent back to the userspace application for display or further use.

## Key Components
- Custom Hardware: A virtual hardware accelerator for image processing.
- QEMU Virtual Environment: Simulated hardware environment for development and testing.
- Linux Kernel Module (LKM): Serves as the interface between the custom hardware and the Linux operating system.
- Yocto Project: Used to build a minimal and tailored Linux image containing the necessary drivers and applications.
- Userspace Application: Acts as the front-end for sending image data to the hardware and displaying the processed output.

## Workflow

- Before diving into our workflow, it’s essential to set up the working environment to ensure a seamless and efficient process without any disruptions. The two primary technologies we will be utilizing are:
- Yocto Project
- QEMU .
- But Before starting our development process we will first make a seprate directory where we can organise our entire development procedure.
    ```bash
        mkdir project
         cd project

## Yocto Project
  - The Yocto Project is designed to enable the development of a Linux OS customized for specific embedded devices.
    It provides powerful tools that not only allow us to tailor Linux to meet our specific requirements but also support
    and streamline the entire development process.

   ### Setting up Yocto Project 
  -  The Yocto Project offers a wide range of capabilities, which are thoroughly documented in the Yocto Project manual. 
     The manual provides comprehensive and well-structured guidance. However, our focus here will
     be limited to aspects specifically related to our project.
     ```bash
       git clone https://github.com/yoctoproject/poky 
##  QEMU 
  - The main idea behind QEMU is to emulate virtual hardware for various architectures (e.g., ARM, RISC-V) different from the host architecture.
    For example, you can emulate an ARM processor along with its entire board on an x86 architecture. The advantage of using QEMU is that it enables quick prototyping of hardware behavior,
    reducing development and testing time. Additionally, it allows us to test our Linux OS on the hardware emulated by QEMU. 
    QEMU supports a wide range of architectures and boards, including Virt, Raspberry Pi, STM32, and many others.

    ###    Setting up QEMU
    ```bash
            # Clone the QEMU source code repository from GitHub into your working directory:
                git clone https://github.com/qemu/qemu
            # With the local copy of the QEMU repository ready, we need to build it from source. 
            # Before building, explore the available configuration options using:
              ./configure --help 
            # This command lists all the options you can use when invoking 'make', allowing QEMU 
            # to include the necessary components for emulating the desired machine.

            # Next, configure the build with the following command:
               ./configure --target-list=arm-softmmu,aarch64-softmmu --enable-slirp
             # Explanation:
             # The 'arm-softmmu' and 'aarch64-softmmu' options are for system-level emulation.
             #'--enable-slirp' enables the SLIRP library, which is required for SSH support, 
             #allowing an SSH connection to the emulated machine.
             # Finally, build QEMU using:
               make 

-  After the configuring and building the QEMU from source we will write the script that will invoke QEMU and emulate our machine
   you can find the script in the repository with the name of:
     ```bash 
       qemu_s.sh
 ### Setting up the hardware
- We are using 