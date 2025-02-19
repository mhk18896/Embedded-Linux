# Development of of custome Linux Kernel Module for a custom hardware model

<p align="center">
  <img src="/images/QEMU-Logo.wine.png" width="200" style="display: inline-block; margin-right: 10px;">
  <img src="/images/Yocto-Linux.png" width="200" style="display: inline-block;">
  <img src="/images/polito.jpg" width="200" style="display: inline-block;">
</p>



## Project Overview

<p align="center">
  <img src="/images/project_over.jpg" width="350" style="display: inline-block; margin-right: 10px;">
</p>


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
  <img src="/images/Screenshot from 2024-12-16 21-04-02.png" width="250" style="display: inline-block;">
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

My hardware source file that i am using in QEMU is [here](pcimatrix.c)

#### QEMU Script
We have to write a bash script for running QEMU and also include our hardware model so that we can integrate our hardware model with emualted system.
I have written a bash script which you can  find in the repository here [QEMU Bash Script](qemu_32.sh).

---
## Software
In this section we will talk about the software stack that we have developed to access our hardware resources.
There are three distinct steps that we have to follow:
- Develop a Linux Operating System using yoctoproject
- Develop a kernel module and userspace application 
- Cross Compile the kernel module and userspace application for that particular kernel using the yoctoproject
### Development Flow
To develop all the above software stack either it is Linux OS,Kernel Module or UserSpace applicaiton the yoctoproject is a one stop shop.
i have uploaded my [KernelModule](lmmodule) and [UserSpace application](capp) files with thier respective yoctoproject configuration files which can also gives you an Idea how 
yoctoproject works.I am not explaining the kernel module and userspace application here because the code that i have uploaded is self explanatory and has all the explanation there.  

---

**''I’ve missed more than 9,000 shots in my career. I’ve lost almost 300 games.
Twenty-six times I’ve been trusted to take the game-winning shot and missed.
I’ve failed over and over and over again in my life. And that is why I succeed.''**

**Michael Jordan**







## Refrences
Here are some of the links that i used to do my work on this project
- (https://github.com/qemu)
- (http://www.youtube.com/@TheYoctoProject)
- (https://www.qemu.org/docs/master/index.html)
- (https://github.com/Johannes4Linux/pci-echodev)
- (https://www.yoctoproject.org/)
