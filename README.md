# OpenD2
A project to open-source Diablo 2, under the GNU General Public License.

### Project Goals
Simply put, this project is a total rewrite of the game engine. It uses the original game files, and uses the original game's save files. Ideally it will also be compatible in TCP/IP games with the original client, but this may not be feasible.

Why would you want this? Well:
 * It will fix some bugs. However it will try to remain as close as possible to the original game experience, and not add too many new features.
 * It is a great base for building mods. In the past, mods relied on reverse engineering the game through hooking and memory patches.
 * It will run better than Blizzard's game, and won't require fiddling with Windows compatibility settings to work.
 * It will run on Linux, Mac and Windows, without the need for emulators (ie, Wine)

It will not support Open or Closed Battle.net in order to minimize legal issues. Also, it will not support the cinematics because those use the proprietary BINK format.

### Project Status
The majority of the gamecode is still being written. At the moment, only some of the menus work. The next major revisions involve work on the level code.

### Compiling
To compile this project, all you will need is CMake.
*Currently the project only compiles and runs on Windows. More rewriting is needed to get this working on other platforms.*
On Windows, Visual Studio 2017 or later is needed in order to compile.

Run cmake-gui and set the Source directory to this folder. Set the "Where to build the files" to be ./Build. (This is so that the git repository doesn't pick this up as a source directory). Then, simply open the project file in whatever IDE you want.

### Running
*Generally speaking* you will want to run the game from a separate directory from the main game, in order to not screw up your original installation.
The original game options are preserved:

 -w - Run in Windowed mode

And OpenD2 adds a few of its own, which start with `+` instead of `-`:

 +basepath="..." - Set the basepath (ie, where your game is installed to). Replace the ... with the path.
 +homepath="..." - Set the homepath (ie, where your game saves data to). Replace the ... with the path. Defaults to `<user>/My Documents/My Games/Diablo II`.
 +modpath="..." - Set the modpath (ie, where mods overwriting content will read from)
 +sdlnoaccel - Disables hardware acceleration
 +borderless - Run in borderless windowed mode
 +logflags=... - Set the priority for logging information. These are flags. 1 = Log Crashes, 2 = Log messages, 4 = Log debug info, 8 = Log system info, 16 = "prettify" the log

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
The clientside is responsible for client logic, mostly with drawing the menus and sprites.