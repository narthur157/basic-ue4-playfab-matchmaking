## Basic PlayFab UE4 Plugin

A starting point for matchmaking in UE4 using PlayFab

This does NOT using Epic's OnlineSubsystem framework. Why?
Online Subsystem is quite complex, and handles a lot of different things.

Ideally, this would use online subsystem, however, using online subsystem is a difficult topic
unto itself. The idea is that you can drop this plugin into a project and be up and running with matchmaking
within an hour or so.

In order to accomplish that, this plugin makes a number of assumptions, such as:
- There is only 1 logged in user
- Matchmaking is done by a single player
- You can only search a single queue

Matches end after a fixed amount of time, but this can be disabled/adjusted in project settings

### What this plugin does

GSDK Integration
Multiplayer 2.0, specifically matchmaking
Simplified login flows

### Setup

You need to install the PlayFab SDK packages from Nuget (these are included in the example project)
Copy the `BasicPlayFab` folder to your `project/Plugins` folder.

In project settings -> Plugins -> Basic PlayFab you can configure various aspects of this plugin

### CONTRIBUTING
Pull requests/issues are welcome. You can also contact me on Discord at narthur157#1290

### Design

The bulk of the work in this plugin is done by a `GameInstanceSubsystem`. The Gameinstance lifecycle allows matchmaking
regardless of which level is currently loaded. This means other maps can be loaded while matchmaking is active.

The UI/ folder contains widget bases which can provide more convenient methods for interfacing with the API via blueprint

### LICENSE
MIT License, see LICENSE.md

The PlayFab plugin itself uses the Apache 2.0 license

### Credits
https://github.com/PlayFab/UnrealMarketplacePlugin
https://gitlab.com/mtuska/OnlineSubsystemPlayFab
