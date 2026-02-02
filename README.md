# Lifeblood Engine 2 🩸

> **High-Performance C++ Arena Shooter Engine**
> *Built for speed, precision, and classic FPS movement.*

Lifeblood Engine is a lightweight, custom-built game engine written in **C++17** and **OpenGL 3.3**. It is designed specifically for creating fast-paced multiplayer arena shooters with "old-school" movement mechanics (Quake/CS style), featuring air acceleration, bunnyhopping, and sliding.

![Engine Status](https://img.shields.io/badge/Status-Active-brightgreen) ![License](https://img.shields.io/badge/License-MIT-blue) ![Platform](https://img.shields.io/badge/Platform-Windows-0078D6)

## ✨ Key Features

### 🚀 Advanced Physics & Movement
*   **Quake-style Movement**: Full implementation of Air Accelerate, Strafing, and Bunnyhopping.
*   **Dynamic Mechanics**: Crouch sliding, Jump Pads, and Jump Mines for verticality.
*   **Precise Collision**: Custom AABB (Axis-Aligned Bounding Box) physics engine.

### 🎨 Modern Rendering Pipeline
*   **OpenGL 3.3 Core Profile**: Efficient, programmable graphics pipeline.
*   **3D Model Support**: Integrated `.obj` loader (via `tinyobjloader`) for weapons, props, and level geometry.
*   **Lighting**: Blinn-Phong lighting model for realistic specular highlights.
*   **HUD**: Lightweight text and 2D rendering system.

### 🛠️ In-Game Map Editor 2.0
*   **Real-time Building**: Construct levels while playing.
*   **Prop System**: Place 3D models (Trees, Crates, Barrels) directly in the world.
*   **Ghost Preview**: Visual placement guide for precision mapping.
*   **Serialization**: Save and load maps to custom formats instantly.

### 🌐 Multiplayer Networking
*   **UDP Architecture**: Low-latency networking custom-built on Winsock.
*   **Host/Join System**: Seamlessly host local servers or connect via IP.
*   **Replication**: Basic player state and movement synchronization.

### 🔫 Gameplay Framework
*   **Weapon System**: Hitscan (AK-47, Deagle, Sniper) and Projectile support.
*   **Audio Engine**: 3D spatial sound integration (via `miniaudio`).
*   **AI Bots**: Basic enemy AI for single-player practice.

---

## 🎮 Controls

### Gameplay
| Key | Action |
| :--- | :--- |
| **W, A, S, D** | Move / Strafe |
| **Space** | Jump / Bhop |
| **Ctrl** | Crouch / Slide |
| **LMB** | Fire Weapon |
| **RMB** | Aim / Scope |
| **1-4** | Switch Weapons |
| **TAB** | Scoreboard (Planned) |

### Map Editor (Press `F1` to Toggle)
| Key | Action |
| :--- | :--- |
| **LMB** | Place Block/Prop |
| **RMB** | Remove Object |
| **T** | Cycle Prop Type (Block -> Tree -> Crate) |
| **Arrows** | Move Ghost Cursor (if needed) |
| **F5** | Save Map |

---

## 🏗️ Building & Installation

### Dependencies
The engine uses **CMake FetchContent** to automatically manage dependencies. You only need:
*   **CMake** (3.10+)
*   **Visual Studio** (or any C++17 compiler)

### Build Instructions
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
Ensure the `assets` folder is in the same directory as the executable (the build script handles this automatically).
```bash
./bin/Release/Lifeblood.exe
```

## 🔗 Connect with me
[![YouTube](https://img.shields.io/badge/YouTube-@adiruaim-FF0000?style=for-the-badge&logo=youtube)](https://www.youtube.com/@adiruaim)
[![TikTok](https://img.shields.io/badge/TikTok-@adiruhs-000000?style=for-the-badge&logo=tiktok)](https://www.tiktok.com/@adiruhs)

### 💰 Legacy Crypto
* **BTC:** `bc1qflvetccw7vu59mq074hnvf03j02sjjf9t5dphl`
* **ETH:** `0xf35Afdf42C8bf1C3bF08862f573c2358461e697f`
* **Solana:** `5r2H3R2wXmA1JimpCypmoWLh8eGmdZA6VWjuit3AuBkq`
* **USDT (TRC20):** `TNgFjGzbGxztHDcSHx9DEPmQLxj2dWzozC`
* **USDT (TON):** `UQC5fsX4zON_FgW4I6iVrxVDtsVwrcOmqbjsYA4TrQh3aOvj`

### 🌍 Support Links
[![Donate](https://img.shields.io/badge/Donate-adiru3.github.io-FF0000?style=for-the-badge)](https://adiru3.github.io/Donate/)
[![Donatello](https://img.shields.io/badge/Support-Donatello-orange?style=for-the-badge)](https://donatello.to/Adiru3)
[![Ko-fi](https://img.shields.io/badge/Ko--fi-Support-blue?style=for-the-badge&logo=kofi)](https://ko-fi.com/adiru)

[![Steam](https://img.shields.io/badge/Steam-Trade-blue?style=for-the-badge&logo=steam)](https://steamcommunity.com/tradeoffer/new/?partner=1124211419&token=2utLCl48)
