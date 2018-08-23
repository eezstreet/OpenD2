# Introduction

OpenD2 was deliberately designed to be as easy to edit as possible, even at the expense of performance. (Such performances are part of a later milestone; see the list of milestones for more information) As part of any open source project, it wouldn't be anywhere without the help of the community.

So, how can you help contribute? Read on...


# Table of Contents

1. Introduction
2. Table of Contents (you are here)
3. Milestones
4. General Guidelines
5. Code Layout
6. Style Guide


# Milestones

OpenD2 has the following milestones:

## Milestone 1: Engine Replacement
In this milestone, the game.exe produced by OpenD2 can serve as a drop-in replacement for the original game's game.exe, perhaps optionally with additional features.

## Milestone 2: Main Menu
In this milestone, the main menu works and the player can start or join a game, create a new character, and view their savegames (and delete them). BINs are loaded. DC6s

## Milestone 3: Basic Game (In progress)
In this milestone, the player can walk (or run) around and view a preset level. Players can see each other in multiplayer. There is collision and pathfinding for movement. BINs are compiled from TXT files.

## Milestone 4: Random Level Generation
In this milestone, levels are randomly generated and stitched together properly. There are multiple levels, and waypoints can be taken between them. There are multiple acts and the player can travel between them using waypoints.

## Milestone 5: Interactions
In this milestone, NPCs can be talked to with their Introduction and Gossip options. Monsters are randomly spawned according to their levels.txt entries. Monsters can be attacked using the player's fists. The attack-defense routine is tightened up. The player can level up and accumulate stat and skill points (but can't use the latter). Most basic monster AI (that don't involve skills) is done, including Fallen, Zombies, Skeletons, Goatmen, ... but NOT missile based AI like Skeleton Archers.

## Milestone 6: Items
In this milestone, items will be added. This includes *all* aspects of items, including trading (between players but also with NPCs), treasure classes and magic prefixes/suffixes and unique items. The Horadric Cube functions. Portals do not function, since they would be based on a skill. Likewise, oskills obviously don't function. Items also start identified, since identification relies on a skill to function.

## Milestone 7: Skills
In this milestone, skills are added. The player can spend their skill points. Missiles function. Monster AI that uses skills and missiles is done. Portals and identification work. Oskills, charged skills, and Chance to Cast item properties work.  

## Milestone 8: Makin' it nice
In this milestone, the remaining things that are left to be done, are done. This includes quests, sounds/music/environmental effects, perspective mode, dynamic, colored lights and shadowing, and making the game run on more platforms. Possibly also including an alternative to Battle.net, like a master server.

## The Future 
At this point, OpenD2 is considered "complete." Future updates and patches will address changes to Diablo 2 that Blizzard does, or compatibility with any Remastered Edition content (if Blizzard makes such a thing). Additionally, I'd like to experiment with making a modder friendly version with more freedom (similar to kingpin's Dreamlands project) or perhaps a version that works with Diablo 1 content.


# General Guidelines

In general, the goal of this project is to replicate the behavior of Diablo 2 to a T, unless that proves to be excessively difficult or impossible. For example, currently OpenD2 uses the same networking protocol as the original game does, but using its own packet structures as an "evolution" from the original game is under consideration, since it's much less effort than researching what each packet does, but you lose network interoperability with the original game.

This also extends to bugs that exist in the original game, although some bugs in the original game *will* be patched. A full list of the bugs and whether or not they will be fixed is pending. 

Undocumented behavior, where the game functions in a way that isn't mentioned in the manual, strategy guide, or ingame will be preserved. Examples of undocumented behavior include (but aren't limited to): Assassins can unlock chests without a key, the Cow King dropping stamina potions, Resist Fire/Cold/Lightning increasing maximum resistances, etc.

Additional moddability *won't* be added if it either breaks compatibility with original TXT/BIN mods (for example, adding extra columns to skills.txt), requires more files in the MPQs, or alters the savegame contents. These are probably best suited by a fork or offshoot version. If it doesn't (like for example, providing support for OGG format sounds, or extending the limits of some TXT files), then go for it!

The project will use as few libraries as possible. Part of the purpose in this project is in developing a full engine from top to bottom. Most of the library functions are abstracted out to the D2Lib namespace, where you can swap in whatever functions you want for speed or optimization purposes. 

In fact, the engine only uses SDL; not STL or Boost. This is deliberate and not changing. Boost is a massive (hundreds of megabytes) dependency. STL is not implemented because its performance is poor and some behavior and availability varies from platform to platform. Both of them have the tendency to make code messy, and clarity and maintainability is important in this project.

Lambda functions and singleton patterns are to be avoided as they lead to hard-to-maintain code.

Templates generally shouldn't be used for anything too complex. Simplicity is better than unneeded flexibility. 


# Code Layout

The core OpenD2 engine can be broken into an engine, and modcode "programs". This idea of two different layers is something that was explored originally in the Quake 2 (idTech 2) engine, and it seems advantageous here. The engine almost functions as a CRT or an operating system, and the modcode runs on top of that as separate programs competing for resources. Hence, any time the modcode wants to access something from the engine, it uses an object called "trap", similar to trap calls in an operating system.

For instance, `trap->Milliseconds` returns how long (in milliseconds) the engine has been running for. (If you were doing this from the engine, you could just call GetMilliseconds or SDL_GetTicks, which is what is used)

## Engine (game)
Provides all of the core game utilities, such as networking, rendering, sound, file I/O, input, localization, commandline argument parsing, etc. The code for this is stored in **/Game/**. All of the code here is platform independent, with the exception of files denoted Platform_X.cpp, where X is a platform. (example: Platform_Windows.cpp).

The main program entrypoint is in diablo2.cpp and you can trace program execution from there.

## "Modcode"
Modcode executes as a separate, dynamically linked library. The idea here is that mods can load their own custom dynamic libraries to execute their own code, based on the OpenD2 framework. The libraries are loaded independently, so you can mismatch the server and client. For example, a special server might have its own custom chat commands for roleplaying.

The engine calls very specific functions in the modcode. Both the server and client have routines for packet read, packet write, initialization, shutdown, and one that is executed per tick. (Note, the server and client may be running at different tickrates!)

Prior to any code being run, GetModuleAPI is executed on the modcode to make sure the engine and modcode are running the same version. It's also where the functions, mentioned above, get assigned.

## Server Modcode (D2Game)
Provides all of the gamecode logic that's executed on the server. This would be things like dropping loot, spawning monsters, etc. The code for this is stored in **/Server/**. All of the code here is platform independent.

## Client Modcode (D2Client)
Provides all of the gamecode logic that's executed on the client. This would be things like the user interface (including the main menu!). The code for this is stored in **/Client/**.

## Common Modcode (D2Common)
Common code that is shared between the client and the server. This would be things like random level generation, BIN file parsing, item stats, and more. The code for this is stored in **/Common/**.

## Shared Code (--)
The code in /Shared/ is compiled into both the engine and the modcode. It's generally not advised to change anything in here unless you're recompiling both the modcode and the engine. The entirety of D2Lib is stored in here. This would be your common math and string functions.


# Style Guide
As has been mentioned above, the goal here is readability, and ease of learning. The style guide is part of that.

## Variable and Parameter Names
Currently the project uses Windows-style Hungarian notation for variable names. This is because some of the code is copied from other sources (with permission!) and integrated smoothly into the engine. camelCase code looks pretty ugly, especially with namespaces, but the case could be made for other variable naming schemes.

## Spacing
Tabs should be used for indenting code, not spaces. Spaces drive up file sizes, are annoying to remove, and don't offer much benefit over tabs.

## Brackets
Brackets {} should be placed on a new line. This is longer, but it makes the code much more readable.

## Namespaces and Functions
Both of these should be given names with capitalized words. Namespaces are nouns, and functions are verbs. Example: `TokenInstance` namespace, and `SetTokenInstanceFrame` function.
