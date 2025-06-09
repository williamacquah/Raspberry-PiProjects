# SphereSockets Project

This repository contains a collection of TCP and UDP client-server programs developed as part of the SphereSockets task. The project is intended for learning and demonstrating socket programming concepts on a Raspberry Pi.

---

## 🚀 Project Overview

The SphereSockets task includes:
- **TCP Client-Server**: A basic TCP server that waits for incoming client connections, and a client that connects and exchanges messages with the server.
- **UDP Client-Server**: A simple UDP server and client pair that allows sending and receiving messages.

The project was tested and verified to work on a Raspberry Pi using Visual Studio Code over SSH.

---

## 📂 Repository Structure

```
/SocketsI/
  ├── daytimeTCPCl.c      (TCP Client source code)
  ├── daytimeTCPSrv.c     (TCP Server source code)
  ├── daytimeUDPCl.c      (UDP Client source code)
  └── daytimeUDPSrv.c     (UDP Server source code)
```

---

## 🛠️ How to Compile and Run

### Prerequisites
- Raspberry Pi with GCC installed
- Visual Studio Code (optional, for easier SSH connection)

### 1️⃣ Compile the Programs

Open a terminal and navigate to the directory containing the files:
```bash
cd /home/pi/VES/SocketsI/
```

Then compile each program:

```bash
# Compile TCP server
gcc daytimeTCPSrv.c -o daytimeTCPSrv

# Compile TCP client
gcc daytimeTCPCl.c -o daytimeTCPCl

# Compile UDP server
gcc daytimeUDPSrv.c -o daytimeUDPSrv

# Compile UDP client
gcc daytimeUDPCl.c -o daytimeUDPCl
```

---

### 2️⃣ Run the Programs

#### 🖥️ TCP Server
In one terminal:
```bash
./daytimeTCPSrv
```

#### 🖥️ TCP Client
In another terminal:
```bash
./daytimeTCPCl
```

You should see the client connect and be able to exchange messages with the server.

---

#### 🖥️ UDP Server
In one terminal:
```bash
./daytimeUDPSrv
```

#### 🖥️ UDP Client
In another terminal:
```bash
./daytimeUDPCl
```

You should see the client connect and be able to send and receive messages with the server.

---

## 💡 Notes

- Always start the **server** first before running the client.
- These programs demonstrate basic socket programming and are intended for educational use.
- Use `Ctrl+C` to stop the server or client when you’re done testing.

---

## 📄 License

This project is open-source and available for learning purposes.