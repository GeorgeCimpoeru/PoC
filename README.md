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

- **GitHub** [link](https://github.com/GheorgheStefanIonut/PoC)
  - **Main Branch**: "master".
  - From "master" we have "development" branch thgat continue with  "DEV_BE" and "DEV_FE" branches.
  - From "DEV_BE" -> feature branches with the following format: feature/<UDS/OTA>_<task_nr> and feature/<UTEST>_<task_nr>.
  - From "DEV_FE" -> feature branches with the following format: feature/<API_call>_<task_nr> and feature/<TEST>_<task_nr>.

- **Branching Schema**: [Branching.drawio](https://app.diagrams.net/#G1mUuN_Tui5mvncnqzlkc9_9VdUXv34oN1#%7B%22pageId%22%3A%22Y1Fage62GiYIG0lpVpXi%22%7D)
- **Documentation for Branching Strategy**: [Branching Strategies for Development Teams](https://connect2grp.medium.com/branching-strategies-for-the-development-teams-95cafd7806c4)

### 2.3 Technologies

- **Languages**:
  - C++ 17 (services, unit tests)
  - JavaScript with React (frontend)
  - Python 3.7 (API)
- **Agile task management**: Trello
  - [Trello Board](https://trello.com/b/zgVJUP5l/poc)
- **Unit tests**: C++ | google test

### 2.4 Communication

- **Connection**: SSH
- **UDS**
- **Update**: OTA (wi-fi), protocol: HTTPS

### 2.5 Tools

Cmake, g++ 9.4.0, gdb 14.2, make 4.2.1, VisualStudio(or other IDEs), Doxygen.

## 3. User interface

### 3.1 API integration

- **OTA/UDS services in endpoints**: [link](https://drive.google.com/file/d/1dZRY9zdOW0Nr5zMv6tEKj2o_tyujudeV/view?usp=sharing)
- **Language**: Python Framework: Flask

### 3.2 Programming language

- **ECUs JavaScript with React.js**

## 4. Software architecture

### 4.1 Services Oriented Architecture (SOA) [link](https://docs.google.com/document/d/1E4xT5WU4ZHL9182GUKhBbhXgD7vMkHU16Dhb_u7bc2Q/edit)

## 5. API endpoints [link](https://docs.google.com/document/d/1xtzPtvo80uxm8mtJV7xgt_EGn7DZCeYMmVLdpTMZlMc/edit)

## 6. Services 

### 6.1 UDS 

Diagnostic session control, ECU reset, Read/Write data by identifier, Authentication, Tester Present, Read Data by Address, Read DTC information, Access Timing Parameters, Negative Response, Clear Diagnostic Information

### 6.2 OTS 

Request download, Request Transfer exit, Transfer Data, Request update status, Checksum, Error handling, Roll back, Negative Response, Data validation

## 7. Hardware design [link](https://app.diagrams.net/#G1pLkDS_JuORKLck-IC-tuyhHPmMGYls3Z#%7B%22pageId%22%3A%22Tmk5czbL90FaX_LbBNeC%22%7D)

## 8. Documentation

All relevant documentation for the project is added on the sharepoint location: [Documentation](https://drive.google.com/drive/folders/1L6onHStktkrQ_jpqi3EpsY5z0OOdmlnY)