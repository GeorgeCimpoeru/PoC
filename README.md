# Proof of Concept (PoC)

## 1. Introduction

This document describes the process for implementing remote diagnosis & OTA with Raspberry Pi.

## 2. Requirements

### 2.1 Technical Environment

- **Hardware**: Raspberry-pi 5 ARM, GRUB for partitioning, Poky
- **Dev OS**: Linux, Ubuntu 20.04
- **ECUs**: simulated in the MCU (sockets)
- **Server**: Google Drive

### 2.2 Versioning

- **GitHub**
  - **Main Branch**: "master".
  - From "master" we start with 2 development branches for "DEV_BE" and "DEV_FE".
  - From "DEV_BE" we have feature branches for each task with the following format feature<UDS/OTA>_<task1> and feature<UTEST>_<task1>.
  - From "DEV_FE" we have feature branches for each task with the following format feature/<API_call>_<task1> and feature/<TEST>_<task1>.

- **Branching Schema**: [Branching.drawio](Branching.drawio)
- **Documentation for Branching Strategy**: [Branching Strategies for Development Teams](https://connect2grp.medium.com/branching-strategies-for-the-development-teams-95cafd7806c4)

### 2.3 Technologies

- **Languages**:
  - C++ 17 (services, unit tests)
  - Java (frontend)
  - Python 3.7 (API)
- **Agile task management**: Trello
  - [Trello Board](https://trello.com/b/zgVJUP5l/poc)
- **Unit tests**: C++ | google test

### 2.4 Communication

- **Connection**: SSH
- **UDS**
- **Update**: OTA (wi-fi), protocol: HTTPS

### 2.5 Prerequisites

Cmake, g++ 9.4.0, gdb 14.2, make 4.2.1, VisualStudio(or other IDEs).

## 3. Documentation

[Documentation](https://drive.google.com/drive/folders/1L6onHStktkrQ_jpqi3EpsY5z0OOdmlnY)