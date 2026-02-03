# 🩸 Lifeblood Engine 2

![License](https://img.shields.io/badge/license-MIT-blue.svg) ![Language](https://img.shields.io/badge/language-C%2B%2B17-blue) ![OpenGL](https://img.shields.io/badge/OpenGL-3.3%2B-green)

**Lifeblood Engine** is a high-performance, multiplayer FPS engine built from scratch in C++. It focuses on classic **Quake-style movement mechanics** (Bunnyhopping, Air Strafing), raw input precision, and a competitive networking architecture.

---

## 🔥 Key Features

### 🏃‍♂️ Advanced Physics & Movement
*   **Quake-Like Movement**: Fully implemented air acceleration, friction, and bunnyhopping mechanics.
*   **2000Hz Simulation**: Physics runs at a fixed high-frequency tickrate for maximum consistency on any framerate.
*   **Configurable Physics**: tweak movement values (Gravity, Speed, Friction) in `assets/physics.cfg` without recompiling.

### ⚔️ Multiplayer & Networking
*   **UDP Networking**: Custom reliable/unreliable packet system.
*   **Client-Server Architecture**: Authoritative server logic.
*   **Anti-Cheat System**: 
    *   **Integrity Hashing**: Server validates client files (`physics.cfg`, models) upon connection.
    *   **Mismatch Rejection**: Clients with modified stats or assets are rejected to prevent cheating.

### � Rendering & Modding
*   **OpenGL 3.3 Core**: Efficient rendering pipeline.
*   **OBJ Model Loader**: Supports loading custom 3D models for Weapons, Map, and Props.
*   **Custom Maps**: Drop your Blender map export as `assets/level.obj` to play on custom levels instantly.
*   **Builder Mode**: In-game tool to place props dynamically.

### 🤖 AI & Gameplay
*   **Smart Bots**: AI that tracks the player and navigates the map.
*   **Combat System**: Hitscan weaponry (AK-47), Health system, Respawning.
*   **3D Spatial Audio**: Implemented using Miniaudio.

---

## 🛠️ Build Instructions

### Prerequisites
*   **CMake** (3.10+)
*   **C++ Compiler** (MSVC, GCC, or Clang)
*   **OpenGL Drivers**

### Compiling
```bash
# Clone the repository
git clone https://github.com/adiru3/Lifeblood-Engine.git
cd Lifeblood-Engine

# Create build directory
mkdir build
cd build

# Configure and Build
cmake ..
cmake --build . --config Release
```

### Running
*   **Single Player**: Launch and select "Single Player".
*   **Host Server**: Run `Lifeblood.exe` -> Single Player (Acts as Listen Server).
*   **Join Server**: Run `Lifeblood.exe` -> Multiplayer -> Enter IP (Default 127.0.0.1).

---

## 🎮 Modding Guide

### Changing Physics
Open `assets/physics.cfg` in any text editor:
```ini
gravity=800.0
max_speed=320.0
air_accelerate=10.0
jump_power=270.0
```
*Note: Clients must have the same config as the server to connect!*

### Custom Maps
1.  Create a model in Blender.
2.  Export as **Wavefront (.obj)**.
3.  Rename to `level.obj`.
4.  Replace the file in `assets/` folder.

---

## 🌍 Support & Credits

Engine developed by **Adiru3** and the Open Source Community.

[![Donate](https://img.shields.io/badge/Donate-adiru3.github.io-FF0000?style=for-the-badge)](https://adiru3.github.io/Donate/)
[![GitHub](https://img.shields.io/badge/GitHub-Adiru3-181717?style=for-the-badge&logo=github)](https://github.com/adiru3)

---
*Lifeblood Engine v1.1 - 2026*
