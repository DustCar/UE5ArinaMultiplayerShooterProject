**Currently this project is a work-in-progress and is the main project I am developing as of 09/05/2024. The contents in this README may be outdated.**

**Many of what I have listed are things that I have already covered**

**Last Update: 08/30/2024**

# Project Purpose
Personal project that I created alongside a UDemy course called "Unreal Engine 5 C++ Multiplayer Shooter" made by Stephen Ulibarri.

Largest reason I decided to take the course is to learn about Multiplayer and how games in Unreal can be developed when including Multiplayer.

# Project Takeaways
In this course, I learned new topics that broadened my skill set with Unreal Engine and C++. Some of these topics include:
- Creating a plugin
- Setting up Networking/Multiplayer through Steam, including hosting and joining game sessions
- Replicating data for multiplayer using RPCs and OnRep functions
- Using game states for player information
- many more...

*_The topics listed are what I have currently covered as of Sep. 5, 2024_

# Personal Additions
In addition to the topics I learned through the course, I added my own additions to try my hand at applying what I have learned.

Here are a few things I added that were not done as part of the course:
- Displayed player names above their character

# Project Showcase

## Multiplayer Plugin
A Game Module plugin that can be used in any project that is meant to be used with Steam. Uses a client-server model with a listen server as host. Includes a simple menu with the ability to host and join active sessions. Client travel map is configurable in Blueprints.

### Functionality
I created a new UGameInstanceSubsystem C++ class, called MultiplayerSessionsSubsystem, which holds the functionality for the custom Online Subsystem class. This class references the World's Online Subsystem its Session Interface to call Session Interface methods for online session functionality.

I created 5 local functions that are used as setup functions for the actual Session Interface functions that are included in the IOnlineSessionPtr interface class (same names but different parameters). In addition to the functions, they each had a delegate and a delegate handle variable that would get added to the session interface's delegate list, which would get triggered after its respective function runs and runs the callback functions bound to them.

All delegates were created during initialization using `CreateUObject()` from their respective structs.

**FUNCTIONS**
---
#### CreateSession
Function: `void CreateSession(int32 NumPublicConnections, FString MatchType)`
- Checks if a current session exists. If so, save _NumPublicConnections_ and _MatchType_ in a member variable for reuse and call local `DestroySession()`.
- Adds delegate to delegate list using `SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate)` and save handle in a _FDelegateHandle_.
- Sets up session settings like _bAllowJoinInProgress_, _NumPublicConnections_, and more using a `TSharedPtr<FOnlineSessionSettings> LatestSessionSettings` variable.
- Grab local player. Calls `SessionInterface->CreateSessions(UniqueNetId, SessionName, SessionSettings)`, passing in unique net id from local player, NAME_GameSession, and _LatestSessionSettings_ dereferenced

**Related Delegate and delegate handle:** `FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate` : `FDelegateHandle CreateSessionCompleteDelegateHandle`

**Delegate callback function:** `void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)`
- Clears the delegate from the delegate list using the delegate handle, if Session Interface is valid.

---

#### FindSessions
Function: `void FindSessions(int32 MaxSearchResults)`
- Adds delegate to delegate list using `SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate)` and save handle in a _FDelegateHandle_.
- Sets up search settings using a `TSharedPtr<FOnlineSessionSearch> LatestSearchSettings` variable.
- Grab local player. Calls `SessionInterface->CreateSessions(UniqueNetId, SearchSettings)`, passing in unique net id from local player, and _LatestSearchSettings_ dereferenced

**Related Delegate and delegate handle:** `FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate` : `FDelegateHandle FindSessionsCompleteDelegateHandle`

**Delegate callback function:** `void OnFindSessionsComplete(bool bWasSuccessful)`
- Clears the delegate from the delegate list using the delegate handle, if Session Interface is valid.

---

#### JoinSession
Function: `void JoinSession(const FOnlineSessionSearchResult& SessionResult)`
- Adds delegate to delegate list using `SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate)` and save handle in a _FDelegateHandle_.
- Grab local player. Calls `SessionInterface->CreateSessions(UniqueNetId, SessionName, SessionResult)`, passing in unique net id from local player, the name of the session, and the session itself

**Related Delegate and delegate handle:** `FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate` : `FDelegateHandle JoinSessionCompleteDelegateHandle`

**Delegate callback function:** `void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)`
- Clears the delegate from the delegate list using the delegate handle, if Session Interface is valid.

---

#### DestroySession
Function: `void DestroySession()`
- Adds delegate to delegate list using `SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate)` and save handle in a _FDelegateHandle_.
- Calls `SessionInterface->DestroySession(SessionName)`, passing in NAME_GameSession

**Related Delegate and delegate handle:** `FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate` : `FDelegateHandle DestroySessionCompleteDelegateHandle`

**Delegate callback function:** `void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)`
- Clears the delegate from the delegate list using the delegate handle, if Session Interface is valid.
- Calls `CreateSession(int32 NPC, FString MT)` again and passes in the _NumPublicConnections_ and _MatchType_ variables from CreateSession() that were saved in private member variables

---

#### StartSession
Function: `void StartSession()`
- Adds delegate to delegate list using `SessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate)` and save handle in a _FDelegateHandle_.
- Calls `SessionInterface->StartSession(SessionName)`, passing in NAME_GameSession

**Related Delegate and delegate handle:** `FOnStartSessionCompleteDelegate StartSessionCompleteDelegate` : `FDelegateHandle StartSessionCompleteDelegateHandle`

**Delegate callback function:** `void OnStartSessionComplete(FName SessionName, bool bWasSuccessful)`
- Clears the delegate from the delegate list using the delegate handle, if Session Interface is valid.

---
### Menu
A UserWidget class that includes functions for UI setup, button functionality, and similar session functions like in _MultiplayerSessionsSubsystems_ but tailored to calling the Session Intrerface functions and actually moving the player.

To use the functions from the Subsystem, the Menu class had to reference the Subsystem. That way the Menu class can start the Session pipeline and call `CreateSession()` on MultiplayerSessionsSubsystems. However, to continue the pipeline, the Menu class needed to know when its functions would be called in order to do the actual travel. 

This was accomplished by declaring custom delegates on the Subsystem class and binding the Menu class functions to it. This essentially allows the Subsystem class to call Menu functions by broadcasting, avoiding a direct reference to the Menu class. This keeps dependency one-way; Menu depends on Subsystem, but Subsystem does not depend on Menu.

Custom delegates added to MultiplayerSessionsSubsystems:
- `FMultiplayerOnCreateSessionComplete MultiplayerCreateSessionCompleteDelegate`: Dynamic Multicast OneParam Delegate
- `FMultiplayerOnFindSessionsComplete MultiplayerFindSessionsCompleteDelegate`: Multicast TwoParam Delegate
- `FMultiplayerOnJoinSessionComplete MultiplayerJoinSessionCompleteDelegate`: Multicast TwoParam Delegate
- `FMultiplayerOnDestroySessionComplete MultiplayerDestroySessionCompleteDelegate`: Dynamic Multicast OneParam Delegate
- `FMultiplayerOnStartSessionComplete MultiplayerStartSessionCompleteDelegate`: Dynamic Multicast OneParam Delegate





