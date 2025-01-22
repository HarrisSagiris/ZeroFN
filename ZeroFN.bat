@echo off
title ProjectZERO-ZeroFN
color 0f

REM Initial menu to choose between specifying a path or installing Fortnite OG
:main_menu
cls
echo ==========================================
echo    ProjectZERO 
echo    Created by root404 and the ZeroFN team
echo ==========================================
echo.
echo Please choose an option:
echo.
echo [1] Specify FortniteOG Path
echo [2] Install Fortnite OG (Downloads Fortnite OG to a folder named 'FortniteOG' in the current directory)
echo [3] Join our Discord Community
echo [4] Exit
echo.
set /p choice="Enter your choice (1-4): "

if "%choice%"=="1" goto specify_path
if "%choice%"=="2" goto install_fortnite_og
if "%choice%"=="3" start https://discord.gg/yCY4FTMPdK && goto main_menu
if "%choice%"=="4" exit
goto main_menu

:install_fortnite_og
cls
echo =====================================
echo    Installing Fortnite OG
echo    Powered by ZeroFN
echo =====================================
echo.
set "DOWNLOAD_URL=https://public.simplyblk.xyz/1.11.zip"
set "INSTALL_DIR=%cd%\FortniteOG"
set "ARCHIVE_NAME=1.11.zip"

if exist "%INSTALL_DIR%" (
    echo The directory "%INSTALL_DIR%" already exists.
    echo Skipping download, verifying files...
    pause
    goto locate_executable
)

echo This procedure depends on your internet speeds..it can take time..
echo Downloading Fortnite OG files from: %DOWNLOAD_URL%
curl -o "%ARCHIVE_NAME%" "%DOWNLOAD_URL%"
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Error: Failed to download files. Please check your internet connection and try again.
    pause
    exit /b 1
)

echo Extracting files to "%INSTALL_DIR%"...
mkdir "%INSTALL_DIR%"
tar -xf "%ARCHIVE_NAME%" -C "%INSTALL_DIR%"
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Error: Failed to extract files. Ensure 'tar' is available or manually extract "%ARCHIVE_NAME%".
    pause
    exit /b 1
)

echo.
echo Fortnite OG successfully installed in "%INSTALL_DIR%".
del "%ARCHIVE_NAME%"
pause
set "GAME_PATH=%INSTALL_DIR%"
goto locate_executable

:specify_path
cls
echo =====================================
echo    Specify FortniteOG Path
echo    Powered by ZeroFN
echo =====================================
echo.
echo Please enter the path to your FortniteOG folder:
echo Example: C:\Users\username\Desktop\ProjectZERO\FortniteOG
echo.
set /p GAME_PATH="Path: "

REM Validate the specified path
if not exist "%GAME_PATH%" (
    echo.
    echo Error: The specified path does not exist: %GAME_PATH%
    echo Please verify the path and try again.
    pause
    goto specify_path
)

:locate_executable
REM Set the exact path for FortniteOG executable
set "GAME_EXE=%GAME_PATH%\FortniteGame\Binaries\Win64\FortniteClient-Win64-Shipping.exe"

REM Validate the executable exists
if not exist "%GAME_EXE%" (
    echo.
    echo Error: Could not locate FortniteClient-Win64-Shipping.exe at:
    echo %GAME_EXE%
    echo.
    echo Please ensure your FortniteOG installation is complete and contains all required files.
    pause
    goto specify_path
)

echo Found Fortnite executable at: %GAME_EXE%
goto menu

:menu
cls
echo =====================================
echo    ProjectZERO
echo    Powered by ZeroFN
echo =====================================
echo.
echo Current Fortnite executable: %GAME_EXE%
echo.
echo [1] Start Server Only
echo [2] Launch Game Only
echo [3] Start Both (Game + Server)
echo [4] Exit 
echo.
set /p choice="Enter your choice (1-4): "

if "%choice%"=="1" goto start_server_only
if "%choice%"=="2" goto launch_game
if "%choice%"=="3" goto start_hybrid
if "%choice%"=="4" exit
goto menu

:start_server_only
cls
echo =====================================
echo    Starting ProjectZERO server only
echo    Powered by ZeroFN
echo =====================================
echo.
echo Server running on 0.0.0.0:7777
echo Using game files from: %GAME_EXE%
echo.
start /B cmd /c python server_script.py
echo Server started in background
pause
goto menu

:launch_game
cls
echo =====================================
echo    Launching Game Client
echo    Powered by ZeroFN
echo =====================================
echo.
cd /d "%~dp0%GAME_EXE%\.."

REM Kill any existing processes
taskkill /f /im FortniteClient-Win64-Shipping.exe >nul 2>&1
taskkill /f /im EasyAntiCheat.exe >nul 2>&1
taskkill /f /im BEService.exe >nul 2>&1
taskkill /f /im FortniteLauncher.exe >nul 2>&1
taskkill /f /im EpicGamesLauncher.exe >nul 2>&1

REM Delete crash logs and temp files
del /f /q "%localappdata%\FortniteGame\Saved\Logs\*.*" >nul 2>&1
del /f /q "%localappdata%\CrashReportClient\*.*" >nul 2>&1
del /f /q "%localappdata%\FortniteGame\Saved\Config\CrashReportClient\*.*" >nul 2>&1

REM Set compatibility flags and bypass settings
reg add "HKCU\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" /v "%GAME_EXE%" /t REG_SZ /d "~ RUNASADMIN DISABLEDXMAXIMIZEDWINDOWEDMODE DISABLETHEMES" /f >nul 2>&1

REM Launch with bypass parameters and auto-login
start "" "%GAME_EXE%" -NOSPLASH -USEALLAVAILABLECORES -NOSOUND -dx11 -AUTH_TYPE=epic -AUTH_LOGIN=ZeroFN -AUTH_PASSWORD=ZeroFN -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -noeac -nobe -fltoken=none -fromfl=none -nolog -NOSSLPINNING -preferredregion=NAE -skippatchcheck -notexturestreaming

echo Fortnite client launched in bypass mode with auto-login!
echo.
timeout /t 3
goto menu

:start_hybrid
cls
echo =====================================
echo    Starting ProjectZERO Hybrid Mode (Game + Server)
echo    Powered by ZeroFN
echo =====================================
echo.
echo Starting server on 0.0.0.0:7777
echo Using game files from: %GAME_EXE%
echo.

REM Start server in background first
start /B cmd /c python server_script.py
timeout /t 2 >nul

cd /d "%~dp0%GAME_EXE%\.."

REM Kill existing processes
taskkill /f /im FortniteClient-Win64-Shipping.exe >nul 2>&1
taskkill /f /im EasyAntiCheat.exe >nul 2>&1
taskkill /f /im BEService.exe >nul 2>&1
taskkill /f /im FortniteLauncher.exe >nul 2>&1
taskkill /f /im EpicGamesLauncher.exe >nul 2>&1

del /f /q "%localappdata%\FortniteGame\Saved\Logs\*.*" >nul 2>&1
del /f /q "%localappdata%\CrashReportClient\*.*" >nul 2>&1
del /f /q "%localappdata%\FortniteGame\Saved\Config\CrashReportClient\*.*" >nul 2>&1

reg add "HKCU\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" /v "%GAME_EXE%" /t REG_SZ /d "~ RUNASADMIN DISABLEDXMAXIMIZEDWINDOWEDMODE DISABLETHEMES" /f >nul 2>&1

REM Launch game with auto-login
start "" "%GAME_EXE%" -NOSPLASH -USEALLAVAILABLECORES -NOSOUND -dx11 -AUTH_TYPE=epic -AUTH_LOGIN=ZeroFN -AUTH_PASSWORD=ZeroFN -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -noeac -nobe -fltoken=none -fromfl=none -nolog -NOSSLPINNING -preferredregion=NAE -skippatchcheck -notexturestreaming

echo Game client launched in bypass mode with auto-login!
echo Server is running in background
echo.
echo Press any key to return to menu...
pause >nul
goto menu

:create_server
(
echo import socket
echo import threading
echo import logging
echo import json
echo import os
echo import time
echo import random
echo from datetime import datetime
echo.
echo logging.basicConfig(
echo     level=logging.INFO,
echo     format='%%(asctime)s - %%(levelname)s - %%(message)s',
echo     handlers=[
echo         logging.FileHandler('server.log'),
echo         logging.StreamHandler()
echo     ]
echo )
echo logger = logging.getLogger('FortniteServer')
echo.
echo class FortniteGameServer:
echo     def __init__(self, game_exe, host='0.0.0.0', port=7777):
echo         self.game_exe = game_exe
echo         self.host = host
echo         self.port = port
echo         self.running = True
echo         self.match_data = {
echo             'match_id': f'match_{int(time.time())}',
echo             'start_time': datetime.now().isoformat(),
echo             'max_players': 100,
echo             'current_players': 0,
echo             'storm_phase': 0,
echo             'storm_timer': 0,
echo             'match_state': 'lobby',  # lobby, loading, in_progress, ended
echo             'bus_path': self.generate_bus_path(),
echo             'safe_zones': self.generate_safe_zones(),
echo             'loot_spawns': self.generate_loot_spawns()
echo         }
echo         self.active_connections = {}
echo         self.player_states = {}
echo         self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
echo         self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
echo         self.lobby_countdown = 60  # 60 second lobby countdown
echo.
echo     def generate_bus_path(self):
echo         # Generate random battle bus path across the map
echo         start_x = random.randint(-10000, 10000)
echo         start_y = random.randint(-10000, 10000)
echo         end_x = random.randint(-10000, 10000)
echo         end_y = random.randint(-10000, 10000)
echo         return {'start': [start_x, start_y], 'end': [end_x, end_y]}
echo.
echo     def generate_safe_zones(self):
echo         # Pre-calculate all storm circles
echo         circles = []
echo         current_radius = 8000  # Initial circle radius
echo         center_x = random.randint(-5000, 5000)
echo         center_y = random.randint(-5000, 5000)
echo         
echo         for i in range(9):  # 9 storm phases
echo             circles.append({
echo                 'center': [center_x, center_y],
echo                 'radius': current_radius
echo             })
echo             # Next circle is ~50% smaller and slightly offset
echo             current_radius = int(current_radius * 0.5)
echo             center_x += random.randint(-300, 300)
echo             center_y += random.randint(-300, 300)
echo         
echo         return circles
echo.
echo     def generate_loot_spawns(self):
echo         # Generate loot spawn points across the map
echo         loot_spawns = []
echo         named_locations = [
echo             {'name': 'Pleasant Park', 'x': -2000, 'y': 3000},
echo             {'name': 'Tilted Towers', 'x': 0, 'y': 0},
echo             {'name': 'Retail Row', 'x': 3000, 'y': 2000},
echo             # Add more named locations
echo         ]
echo         
echo         for location in named_locations:
echo             # Generate multiple loot spawns around each named location
echo             for _ in range(20):
echo                 x = location['x'] + random.randint(-500, 500)
echo                 y = location['y'] + random.randint(-500, 500)
echo                 loot_type = random.choice(['weapon', 'ammo', 'healing', 'shield', 'materials'])
echo                 loot_spawns.append({
echo                     'position': [x, y],
echo                     'type': loot_type,
echo                     'tier': random.randint(1, 5)
echo                 })
echo         
echo         return loot_spawns
echo.
echo     def start(self):
echo         try:
echo             self.server_socket.bind((self.host, self.port))
echo             self.server_socket.listen(100)
echo             logger.info(f'Fortnite server started on {self.host}:{self.port}')
echo             
echo             # Start lobby countdown thread
echo             lobby_thread = threading.Thread(target=self.lobby_timer)
echo             lobby_thread.daemon = True
echo             lobby_thread.start()
echo             
echo             # Start storm timer thread
echo             storm_thread = threading.Thread(target=self.storm_timer)
echo             storm_thread.daemon = True
echo             storm_thread.start()
echo             
echo             while self.running:
echo                 try:
echo                     client_socket, address = self.server_socket.accept()
echo                     if self.match_data['match_state'] != 'lobby':
echo                         client_socket.send(json.dumps({'type': 'error', 'message': 'Match in progress'}).encode())
echo                         client_socket.close()
echo                         continue
echo                         
echo                     if self.match_data['current_players'] >= self.match_data['max_players']:
echo                         client_socket.send(json.dumps({'type': 'error', 'message': 'Server full'}).encode())
echo                         client_socket.close()
echo                         continue
echo                     
echo                     self.active_connections[address] = client_socket
echo                     self.match_data['current_players'] += 1
echo                     
echo                     client_thread = threading.Thread(
echo                         target=self.handle_client,
echo                         args=(client_socket, address)
echo                     )
echo                     client_thread.daemon = True
echo                     client_thread.start()
echo                     
echo                     logger.info(f'New player connected from {address}')
echo                     self.broadcast_lobby_state()
echo                     
echo                 except socket.error as e:
echo                     if not self.running:
echo                         break
echo                     logger.error(f'Socket error: {e}')
echo                     
echo         except Exception as e:
echo             logger.error(f'Server error: {e}')
echo         finally:
echo             self.shutdown()
echo.
echo     def lobby_timer(self):
echo         while self.running and self.match_data['match_state'] == 'lobby':
echo             if self.match_data['current_players'] >= 2:  # Minimum 2 players to start
echo                 while self.lobby_countdown > 0:
echo                     self.broadcast_to_all({
echo                         'type': 'lobby_countdown',
echo                         'time_remaining': self.lobby_countdown
echo                     })
echo                     time.sleep(1)
echo                     self.lobby_countdown -= 1
echo                 
echo                 # Start the match
echo                 self.match_data['match_state'] = 'loading'
echo                 self.broadcast_to_all({
echo                     'type': 'match_starting',
echo                     'bus_path': self.match_data['bus_path'],
echo                     'initial_zone': self.match_data['safe_zones'][0]
echo                 })
echo                 time.sleep(5)  # Loading screen time
echo                 self.match_data['match_state'] = 'in_progress'
echo                 break
echo             time.sleep(1)
echo.
echo     def broadcast_lobby_state(self):
echo         self.broadcast_to_all({
echo             'type': 'lobby_update',
echo             'players': list(self.player_states.values()),
echo             'countdown': self.lobby_countdown,
echo             'min_players_needed': 2,
echo             'server_info': 'Powered by ZeroFN'  # Added ZeroFN credit
echo         })
echo.
echo     def handle_client(self, client_socket, address):
echo         try:
echo             while self.running:
echo                 data = client_socket.recv(8192)
echo                 if not data:
echo                     break
echo                     
echo                 try:
echo                     message = json.loads(data.decode())
echo                     response = self.process_message(message, address)
echo                     if response:
echo                         client_socket.send(json.dumps(response).encode())
echo                         
echo                     # Broadcast updates to other players
echo                     if message.get('type') in ['position', 'action', 'build', 'combat']:
echo                         self.broadcast_update(message, address)
echo                         
echo                 except json.JSONDecodeError:
echo                     logger.error(f'Invalid message format from {address}')
echo                     
echo         except Exception as e:
echo             logger.error(f'Error handling client {address}: {e}')
echo         finally:
echo             self.remove_client(address)
echo.
echo     def process_message(self, message, address):
echo         message_type = message.get('type')
echo         
echo         if message_type == 'join':
echo             player_data = {
echo                 'id': message.get('player_id'),
echo                 'name': message.get('player_name'),
echo                 'position': message.get('position', [0, 0, 0]),
echo                 'health': 100,
echo                 'shield': 0,
echo                 'inventory': [],
echo                 'materials': {'wood': 0, 'brick': 0, 'metal': 0},
echo                 'eliminations': 0,
echo                 'state': 'lobby'  # lobby, in_bus, skydiving, playing, eliminated
echo             }
echo             self.player_states[address] = player_data
echo             return {
echo                 'type': 'join_success',
echo                 'match_id': self.match_data['match_id'],
echo                 'match_state': self.match_data['match_state'],
echo                 'player_count': self.match_data['current_players'],
echo                 'storm_phase': self.match_data['storm_phase'],
echo                 'storm_timer': self.match_data['storm_timer'],
echo                 'map_data': {
echo                     'bus_path': self.match_data['bus_path'],
echo                     'safe_zones': self.match_data['safe_zones'],
echo                     'loot_spawns': self.match_data['loot_spawns']
echo                 },
echo                 'server_info': 'Powered by ZeroFN'  # Added ZeroFN credit
echo             }
echo             
echo         elif message_type == 'position':
echo             if address in self.player_states:
echo                 self.player_states[address]['position'] = message['position']
echo                 return {'type': 'position_ack'}
echo                 
echo         elif message_type == 'combat':
echo             if address in self.player_states:
echo                 self.handle_combat(message, address)
echo                 return {'type': 'combat_ack'}
echo                 
echo         elif message_type == 'build':
echo             if address in self.player_states:
echo                 self.handle_building(message, address)
echo                 return {'type': 'build_ack'}
echo                 
echo         elif message_type == 'loot':
echo             if address in self.player_states:
echo                 self.handle_looting(message, address)
echo                 return {'type': 'loot_ack'}
echo                 
echo         return None
echo.
echo     def handle_combat(self, message, address):
echo         weapon = message.get('weapon')
echo         target = message.get('target')
echo         hit_location = message.get('hit_location', 'body')
echo         
echo         if target in self.player_states:
echo             # Calculate damage based on weapon and hit location
echo             base_damage = {
echo                 'assault_rifle': 35,
echo                 'shotgun': 95,
echo                 'smg': 17,
echo                 'sniper': 105
echo             }.get(weapon, 10)
echo             
echo             if hit_location == 'head':
echo                 damage = base_damage * 2
echo             else:
echo                 damage = base_damage
echo                 
echo             # Apply damage to shield first, then health
echo             shield = self.player_states[target]['shield']
echo             if shield > 0:
echo                 absorbed = min(shield, damage)
echo                 self.player_states[target]['shield'] -= absorbed
echo                 damage -= absorbed
echo                 
echo             if damage > 0:
echo                 self.player_states[target]['health'] -= damage
echo                 
echo             # Check for elimination
echo             if self.player_states[target]['health'] <= 0:
echo                 self.handle_elimination(target, address)
echo.
echo     def handle_elimination(self, eliminated_address, eliminator_address):
echo         self.player_states[eliminator_address]['eliminations'] += 1
echo         self.player_states[eliminated_address]['state'] = 'eliminated'
echo         
echo         # Drop eliminated player's loot
echo         position = self.player_states[eliminated_address]['position']
echo         loot = self.player_states[eliminated_address]['inventory']
echo         
echo         self.broadcast_to_all({
echo             'type': 'elimination',
echo             'eliminated': self.player_states[eliminated_address]['name'],
echo             'eliminator': self.player_states[eliminator_address]['name'],
echo             'loot_position': position,
echo             'dropped_items': loot
echo         })
echo         
echo         # Check for victory royale
echo         players_alive = sum(1 for p in self.player_states.values() if p['state'] == 'playing')
echo         if players_alive == 1:
echo             winner = next(addr for addr, p in self.player_states.items() if p['state'] == 'playing')
echo             self.handle_victory(winner)
echo.
echo     def handle_victory(self, winner_address):
echo         self.match_data['match_state'] = 'ended'
echo         winner_data = self.player_states[winner_address]
echo         
echo         self.broadcast_to_all({
echo             'type': 'victory_royale',
echo             'winner': winner_data['name'],
echo             'eliminations': winner_data['eliminations'],
echo             'match_stats': {
echo                 'duration': int(time.time() - time.mktime(datetime.strptime(self.match_data['start_time'], "%Y-%m-%dT%H:%M:%S.%f").timetuple())),
echo                 'total_players': len(self.player_states)
echo             },
echo             'server_info': 'Powered by ZeroFN'  # Added ZeroFN credit
echo         })
echo         
echo         # Reset server after brief delay
echo         threading.Timer(10.0, self.reset_server).start()
echo.
echo     def reset_server(self):
echo         self.match_data['match_state'] = 'lobby'
echo         self.match_data['current_players'] = 0
echo         self.match_data['storm_phase'] = 0
echo         self.match_data['storm_timer'] = 0
echo         self.match_data['bus_path'] = self.generate_bus_path()
echo         self.match_data['safe_zones'] = self.generate_safe_zones()
echo         self.match_data['loot_spawns'] = self.generate_loot_spawns()
echo         self.lobby_countdown = 60
echo         self.player_states.clear()
echo         self.active_connections.clear()
echo.
echo     def handle_building(self, message, address):
echo         build_type = message.get('build_type')  # wall, floor, ramp, pyramid
echo         position = message.get('position')
echo         material = message.get('material')  # wood, brick, metal
echo         
echo         if self.player_states[address]['materials'][material] >= 10:
echo             self.player_states[address]['materials'][material] -= 10
echo             
echo             build_data = {
echo                 'type': 'build_placed',
echo                 'build_type': build_type,
echo                 'position': position,
echo                 'material': material,
echo                 'health': 100,
echo                 'owner': self.player_states[address]['name']
echo             }
echo             
echo             self.broadcast_to_all(build_data)
echo.
echo     def handle_looting(self, message, address):
echo         item = message.get('item')
echo         position = message.get('position')
echo         
echo         if len(self.player_states[address]['inventory']) < 5:
echo             self.player_states[address]['inventory'].append(item)
echo             
echo             # Remove item from world
echo             self.broadcast_to_all({
echo                 'type': 'item_picked_up',
echo                 'item': item,
echo                 'position': position,
echo                 'player': self.player_states[address]['name']
echo             })
echo.
echo     def storm_timer(self):
echo         while self.running:
echo             if self.match_data['match_state'] == 'in_progress':
echo                 if self.match_data['storm_phase'] < 9:
echo                     if self.match_data['storm_timer'] <= 0:
echo                         self.match_data['storm_phase'] += 1
echo                         self.match_data['storm_timer'] = 120  # 2 minutes per phase
echo                         
echo                         self.broadcast_to_all({
echo                             'type': 'storm_update',
echo                             'phase': self.match_data['storm_phase'],
echo                             'timer': self.match_data['storm_timer'],
echo                             'next_zone': self.match_data['safe_zones'][self.match_data['storm_phase']]
echo                         })
echo                         
echo                         # Apply storm damage
echo                         self.apply_storm_damage()
echo                         
echo                     self.match_data['storm_timer'] -= 1
echo                 time.sleep(1)
echo.
echo     def apply_storm_damage(self):
echo         current_zone = self.match_data['safe_zones'][self.match_data['storm_phase']]
echo         for addr, player in self.player_states.items():
echo             if player['state'] != 'playing':
echo                 continue
echo                 
echo             pos = player['position']
echo             center = current_zone['center']
echo             distance = ((pos[0] - center[0])**2 + (pos[1] - center[1])**2)**0.5
echo             
echo             if distance > current_zone['radius']:
echo                 storm_damage = 1 + self.match_data['storm_phase']  # Increasing storm damage
echo                 player['health'] -= storm_damage
echo                 
echo                 if player['health'] <= 0:
echo                     self.handle_elimination(addr, None)  # Storm elimination
echo.
echo     def broadcast_update(self, message, sender):
echo         message['timestamp'] = time.time()
echo         encoded_message = json.dumps(message).encode()
echo         
echo         for addr, client in self.active_connections.items():
echo             if addr != sender:  # Don't send back to sender
echo                 try:
echo                     client.send(encoded_message)
echo                 except:
echo                     self.remove_client(addr)
echo.
echo     def broadcast_to_all(self, message):
echo         encoded_message = json.dumps(message).encode()
echo         for client in self.active_connections.values():
echo             try:
echo                 client.send(encoded_message)
echo             except:
echo                 continue
echo.
echo     def remove_client(self, address):
echo         if address in self.active_connections:
echo             try:
echo                 self.active_connections[address].close()
echo             except:
echo                 pass
echo             del self.active_connections[address]
echo             if address in self.player_states:
echo                 del self.player_states[address]
echo             self.match_data['current_players'] -= 1
echo             logger.info(f'Player from {address} disconnected')
echo             self.broadcast_lobby_state()
echo.
echo     def shutdown(self):
echo         logger.info('Shutting down Fortnite server...')
echo         self.running = False
echo         
echo         for addr in list(self.active_connections.keys()):
echo             self.remove_client(addr)
echo             
echo         try:
echo             self.server_socket.close()
echo         except:
echo             pass
echo            
echo         logger.info('Fortnite server shutdown complete')
echo.
echo if __name__ == '__main__':
echo     server = FortniteGameServer(r'%GAME_EXE%')
echo     try:
echo         server.start()
echo     except KeyboardInterrupt:
echo         server.shutdown()
) > server_script.py

REM Check Python installation
python --version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: Python is not installed
    echo Please install Python 3.7 or higher
    pause
    exit /b 1
)

REM Create logs directory
if not exist "logs" mkdir logs

REM Run the server
python server_script.py

echo.
echo Server stopped
echo Press any key to return to menu...
pause >nul
goto menu
