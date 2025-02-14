# ZeroFN - Fortnite Private Server
[![Node.js](https://img.shields.io/badge/Node.js-20.x-339933?style=for-the-badge&logo=nodedotjs&logoColor=white)](https://nodejs.org/)
[![Express.js](https://img.shields.io/badge/Express.js-4.x-000000?style=for-the-badge&logo=express&logoColor=white)](https://expressjs.com/)
[![TypeScript](https://img.shields.io/badge/TypeScript-5.x-007ACC?style=for-the-badge&logo=typescript&logoColor=white)](https://www.typescriptlang.org/)
[![Windows](https://img.shields.io/badge/Windows-10%2B-0078D6?style=for-the-badge&logo=windows&logoColor=white)](https://www.microsoft.com/windows)
[![Docker](https://img.shields.io/badge/Docker-24.x-2496ED?style=for-the-badge&logo=docker&logoColor=white)](https://www.docker.com/)
[![Discord](https://img.shields.io/badge/Discord-Join%20Us!-5865F2?style=for-the-badge&logo=discord&logoColor=white)](https://discord.gg/yCY4FTMPdK)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge&labelColor=black)](https://opensource.org/licenses/MIT)
[![Fortnite](https://img.shields.io/badge/Fortnite-C1S2-2F2D2E?style=for-the-badge&logo=fortnite&logoColor=white)](https://www.epicgames.com/fortnite/)

ZeroFN is a custom Fortnite private server and launcher that allows you to play Fortnite Chapter 1 Season 2. It includes a DLL injection system for authentication bypass and a backend server to handle game requests.

---

## Features

1. **Custom Launcher**
   - User-friendly GUI interface
   - Automatic Fortnite path detection and validation
   - DLL injection system for auth bypass
   - Process monitoring and management

2. **Authentication System** 
   - Full authentication bypass via DLL injection
   - Intercepts and redirects Epic/Fortnite API requests
   - Simulates proper auth tokens and responses
   - No Epic Games login required

3. **Backend Server**
   - Local Express.js server (port 3000)
   - Handles all game API requests
   - Provides cosmetics and profile data
   - Maintains Chapter 1 Season 2 version info

4. **Game Features**
   - Chapter 1 Season 2 version (2.4.2-CL-3870737)
   - Basic cosmetics system
   - Profile management
   - Quest system support

---

## Installation

### Public Release Installation
1. **Download the Public Release**
   - Go to the [Releases](https://github.com/ZeroFN/ZeroFN/releases) page
   - Download the latest `ZeroFN-Public.zip`
   - Extract the ZIP file to a folder of your choice

2. **Install Dependencies**
   - Install [Node.js 20.x](https://nodejs.org/) or higher
   - Install [Visual C++ Redistributable 2015-2022](https://aka.ms/vs/17/release/vc_redist.x64.exe)

3. **Launch ZeroFN**
   - Run `ZeroFNLauncher.exe` from the extracted folder
   - Follow the setup wizard to configure your installation

### Discord Release Installation
1. **Download the Latest Release**
   - Get the latest ZeroFN release from our Discord
   - Extract all files to a folder

2. **Requirements**
   - Windows 64-bit OS
   - Fortnite Chapter 1 Season 2 installation
   - Administrator privileges (for DLL injection)

3. **Setup**
   - Run ZeroFNLauncher.exe
   - Select your Fortnite installation folder
   - The launcher will validate the installation

---

## Usage

1. **Starting the Server**
   - Click the "Start" button in the launcher
   - The launcher will:
     - Start the backend server
     - Launch Fortnite
     - Inject the auth bypass DLL
     - Monitor the game process

2. **Playing the Game**
   - Once Fortnite launches, you can play normally
   - The auth bypass system handles all Epic/Fortnite API requests
   - Your profile data is stored locally

3. **Stopping**
   - Use the "Stop" button to:
     - Close Fortnite
     - Stop the backend server
     - Clean up processes

---

## Troubleshooting

1. **DLL Injection Fails**
   - Run launcher as Administrator
   - Check antivirus isn't blocking zerofn.dll
   - Verify Fortnite process is running

2. **Server Connection Issues**
   - Ensure ports 3000 and 3001 aren't in use
   - Check firewall settings
   - Verify backend server is running

3. **Invalid Path Errors**
   - Select correct Fortnite installation folder
   - Verify FortniteClient-Win64-Shipping.exe exists
   - Check file permissions

---

## Technical Details

- Backend: Node.js/Express (zerofn server)
- TCP Server: Custom protocol (server)
- DLL: Dynamic injection with WinAPI hooks
- Launcher: Native Win32 GUI application

---

## Support

Join our [Discord Server](https://discord.gg/yCY4FTMPdK) for:
- Technical support
- Updates and announcements
- Community discussions
- Bug reports

---

## Legal

This project is NOT affiliated with Epic Games.
All Fortnite assets and properties belong to Epic Games, Inc.
Created by @Devharris and @Ryneex
