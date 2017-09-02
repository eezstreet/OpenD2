# OpenD2
A project to open-source Diablo 2, under the GNU General Public License.

### Project Goals
Simply put, this project aims to be a "drag and drop" replacement for game.exe and the game libraries. It (hopefully) will operate with the original MPQ files and the original file formats. The main purpose of this project isn't to provide new features, but rather a solid base to work off of. Very few bugs of the original game, including the lying character sheet, are going to be addressed in this repository; instead you will want to look to branches and forks to provide those.

Ideally the project will maintain compatibility with the original game *for at least one version* (the exact version still to be decided) over TCP/IP and Open Battle.net. Closed Battle.net will not be supported in order to minimize legal risk.

### Project Status
Currently, you can compile the game.exe module and it will run the original game without issues.

However, very little of the game is actually rewritten. It calls the original libraries (D2Win, Fog, Storm, etc) that the game uses. In due time, these will all be replicated.

### Compiling
To compile this project, all you will need is CMake.
*Currently the project only compiles and runs on Windows. More rewriting is needed to get this working on other platforms.*
On Windows, Visual Studio 2013 or later is needed in order to compile, as some C++11 constructs are used.

Run cmake-gui and set the Source directory to this folder. Set the "Where to build the files" to be ./Build. (This is so that the git repository doesn't pick this up as a source directory). Then, simply open the project file in whatever IDE you want.

### Architecture
Just as in the original game, there are several interlocking components driving the game. The difference is that all but the core can be swapped out by a mod.

#### Core (game.exe)
The core game engine communicates with all of the other components and drives everything. It is (or will be) responsible for the following:
- Window management
- Filesystem
- Memory management
- Log management
- Archive (.mpq) management
- Networking
- Sound
- Rendering

#### Common Code (D2Common.dll)
D2Common contains common routines needed by both the serverside and clientside. This includes things such as dungeon-building from random seeds, skill logic, .TXT -> .BIN compilation, and more.

#### Serverside (D2Game.dll)
The serverside is responsible for quest management, AI, and more. Ideally, this should be allowed to mismatch the client DLL and have custom game server logic.

#### Clientside (D2Client.dll)
The clientside is responsible for client logic. It's not loaded at the start, however.