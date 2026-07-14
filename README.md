# Project GTA Setup & Execution Guide

This repository contains the foundational scaffolding for an open-world action-adventure game. It is split into two components:
1. **GameClient**: The Unreal Engine 5.4+ C++ Gameplay project.
2. **GameServer**: The Node.js/TypeScript Express backend server managing persistent database transactions (saves, logins) via PostgreSQL and session variables via Redis.

---

## 📂 Project Architecture

```
GTA/
├── GameClient/                # Unreal Engine 5 Project
│   ├── GTA.uproject           # UE5 Project Descriptor
│   └── Source/
│       └── GTA/
│           ├── GTA.Build.cs   # Build rules
│           ├── GTA.h / GTA.cpp
│           ├── Public/        # C++ Headers
│           │   ├── GTACharacter.h
│           │   ├── GTAPlayerController.h
│           │   └── GTAGameMode.h
│           └── Private/       # C++ Implementations
│               ├── GTACharacter.cpp
│               ├── GTAPlayerController.cpp
│               └── GTAGameMode.cpp
└── GameServer/                # Node.js + TS Database State Server
    ├── docker-compose.yml     # PostgreSQL & Redis container definitions
    ├── package.json
    ├── tsconfig.json
    └── src/
        ├── index.ts           # Boot loader
        ├── db.ts              # Database connection pools & tables configuration
        └── routes/
            ├── auth.ts        # Register/Login
            └── save.ts        # GET/POST coordinates & metrics
```

---

## 🎮 GameClient (Unreal Engine 5) Setup

1. Make sure you have **Unreal Engine 5.4+** installed via the Epic Games Launcher.
2. Install **Visual Studio 2022** (Windows) or **Xcode** (macOS) with the "Desktop development with C++" and "Game Development with C++" packages.
3. Right-click `GameClient/GTA.uproject` and select **Generate Visual Studio project files** (on Windows) or open the project directly inside the Unreal Engine Editor, which will automatically prompt you to compile missing module binaries.
4. Set the Default GameMode in project settings to `GTAGameMode`.

---

## 🖥️ GameServer (Backend Database & Auth) Setup

### Prerequisites
- [Docker & Docker Desktop](https://www.docker.com/)
- [Node.js v18+](https://nodejs.org/)

### Quick Start
1. Spin up the Postgres and Redis databases:
   ```bash
   cd GameServer
   docker-compose up -d
   ```
2. Install dependencies:
   ```bash
   npm install
   ```
3. Run the development server:
   ```bash
   npm run dev
   ```
   The database tables will automatically be migrated, and the server will listen on port `3000`.

---

## ⚡ API Endpoints Expose Reference
- `POST /auth/register` - Create user and default database slot coordinates.
- `POST /auth/login` - Login check.
- `GET /save/:userId` - Retrieve player position and stats.
- `POST /save/:userId` - Update player coordinates `pos_x`, `pos_y`, `pos_z`, `rot_yaw`, `health`, `armor`, and `money`.
