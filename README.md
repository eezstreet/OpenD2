# OpenD2
A project to open-source Diablo 2, under the GNU General Public License.

![Diablo II Main Menu in OpenD2](https://i.imgur.com/RFNbRiT.png)

### Project Goals
Simply put, this project is a total rewrite of the game engine. It uses the original game files, and uses the original game's save files. Ideally it will also be compatible in TCP/IP games with the original client, but this may not be feasible.

Why would you want this? Well:
 * It will fix some bugs. However it will try to remain as close as possible to the original game experience.
 * It is a great base for building mods. In the past, mods relied on reverse engineering the game through hooking and memory patches to create advanced features.
 * It will run better than Blizzard's game, and won't require fiddling with Windows compatibility settings or running as Administrator to work.
 * It will run on Linux, Mac and Windows, without the need for emulators (ie, Wine)

It will not support Open or Closed Battle.net in order to minimize legal issues. Also, it will not support the cinematics because those use the proprietary BINK format.

### Project Status / Contributing
The majority of the gamecode is still being written. Currently, you can connect to a TCP/IP game and it will show up on the other end that you've connected, however it will stall on loading. Most of the main menu works outside of that.

If you would like to contribute to this project, please fork it and submit pull requests. 


### Compiling

#### Windows
To compile this project on Windows, all you will need is CMake and Visual Studio 2017 or later.

Run cmake-gui and set the Source directory to this folder. Set the "Where to build the files" to be ./Build. (This is so that the git repository doesn't pick this up as a source directory). Then, simply open the project file in whatever IDE you want.

#### Linux
Compilation on linux requires only the SDL runtime to be installed.

After that, compile using these commands:

	cmake .
	make

### Running
*Generally speaking* you will want to run the game from a separate directory from the main game, in order to not screw up your original installation.
The original game options are preserved:

* `-w` - Run in Windowed mode

And OpenD2 adds a few of its own, which start with `+` instead of `-`:

* `+basepath="..."` - Set the basepath (ie, where your game is installed to). Replace the ... with the path.
* `+homepath="..."` - Set the homepath (ie, where your game saves data to). Replace the ... with the path. Defaults to `<user>/My Documents/My Games/Diablo II`.
* `+modpath="..."` - Set the modpath (ie, where mods overwriting content will read from)
* `+sdlnoaccel` - Disables hardware acceleration
* `+borderless` - Run in borderless windowed mode
* `+logflags=...` - Set the priority for logging information. These are flags. 1 = Log Crashes, 2 = Log messages, 4 = Log debug info, 8 = Log system info, 16 = "prettify" the log

*Generally speaking*, you will want to run with `+basepath="C:/Program Files (x86)/Diablo II"` (assuming you have the default install directory)

In order to play, you must host a game in TCP/IP in vanilla Diablo 2 (version 1.10) and join it through the OpenD2 client. This is because OpenD2 does not have a serverside yet.

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
