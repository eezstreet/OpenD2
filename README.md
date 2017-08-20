# OpenD2
A project to open-source Diablo 2, under the GNU General Public License.

### Project Goals
Simply put, this project aims to be a "drag and drop" replacement for game.exe and the game libraries. It (hopefully) will operate with the original MPQ files and the original file formats. The main purpose of this project isn't to provide new features, but rather a solid base to work off of. Very few bugs of the original game, including the lying character sheet, are going to be addressed in this repository; instead you will want to look to branches and forks to provide those.

Ideally the project will maintain compatibility with the original game *for at least one version* (the exact version still to be decided) over TCP/IP and Open Battle.net. Closed Battle.net will not be supported in order to minimize legal risk.

### Compiling
TBD

### Architecture
Just as in the original game, there are several interlocking components driving the game. The difference is that all but the core can be swapped out by a mod.

#### Core (game.exe)
The core game engine communicates with all of the other components and drives everything. It's also responsible for loading and running mods.

#### Graphics (D2GFX.dll)
The graphics engine draws all of the sprites, handles lighting, and provides some basic postprocessing.

#### Sound (D2Sound.dll)
The sound engine is responsible for playing sound effects, music, speech, etc.

#### Networking (D2Net.dll)
The networking component is responsible for packet transmission, serialization, etc. over TCP/IP. It's not responsible for anything related to Battle.net.

#### Game Logic: Shared (D2Common.dll)
The shared logic component provides a simulation state for the client and server to both run off of. D2Common routines involve pathfinding, dungeon building (DRLG), items, etc.

#### Game Logic: Server (D2Game.dll)
The server logic component handles all of the game logic that happens on the server. This includes treasure classes (loot), artificial intelligence, etc.

#### Game Logic: Client (D2Client.dll)
The client logic component handles all of the game logic that happens on the client. This includes drawing the user interface.

