Steam Tools plugin
-------
A collection of tools that integrate with steam, current features:
- Version numbering based on steam build ID
- Steam Input API support that fully integrates with the enhanced input system

## Version Numbering
The SteamToolsCore module provides general functionality, the Blueprint Function Library class "SteamTools" provides a function for retrieving a dynamically constructed Build version based on the steam build ID and project version
The version will be in the form of: \<Project Version>.\<Build ID> for builds that are connected to the steam api, for builds where the steam api is unavailable the Build ID will be replaced by "Developer".

## Steam Input API Support
The SteamToolsInput module provides general functionality for the steam input api with full support for the Enhanced Input System.
This module is meant to be a replacement of the SteamController module that is part of unreal.
This module works by creating custom keys for every registered Action which can be bound as any regular key and work the exact same.

**Config:**\
The Input module needs to be configured in the project settings under: "Plugins">"Steam Input Settings"

Keys:
- This is where you add new Input actions that exist in your controller config, every key has a name and a Key Type.
- The Name is the internal name of the action exactly how it appears in your steam config
- The Key Type should be Digital for "Button", Analog for "AnalogTrigger" and Joystick or MouseInput for "StickPadGyro", MouseInput keys will be handled as if they are mouse inputs

MenuCategory:
- This is the name of the key category that your keys will be generated under
- avoid using names of categories that already exist as any time that the keys need to be regenerated it deletes all keys under the category

For every entry in Keys a set of Keys get generated that you can use in the engine/game to bind to anything that would accept it,
These keys work the exact same as any other key and receive the appropriate input events.

KeyEventRules:
- The bindings slate uses for Up, Down, Left, Right, Previous and Next navigation
- These settings are exposed since the module makes it so no regular controller input gets received while steam is active
- exposing these settings allows users to use the steam input for widgets as well without having to implement any "hacks" such as turning steam input actions back into button actions

KeyActionRules:
- Same as KeyEventRules except controls Accept and Back actions
- why are these seperated? I have no clue, ask slate