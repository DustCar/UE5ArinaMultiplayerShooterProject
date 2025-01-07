**Currently this project is a work-in-progress and is the main project I am developing as of 09/05/2024. The contents in this README may be outdated.**

**Many of what I have listed are things that I have already covered**

**Last Update: 01/07/2025**

# Project Purpose
Personal project that I created alongside a UDemy course called "Unreal Engine 5 C++ Multiplayer Shooter" made by Stephen Ulibarri.

Largest reason I decided to take the course is to learn about Multiplayer and how games in Unreal can be developed when including Multiplayer.

# Project Takeaways
In this course, I learned new topics that broadened my skill set with Unreal Engine and C++. Some of these topics include:
- Creating a plugin
- Setting up Networking/Multiplayer through Steam, including hosting and joining game sessions (works only regionally on Steam)
- Replicating data for multiplayer using RPCs and OnRep functions
- Using game state and player state for player information
- Adding simple animations and blending them using Animation Blueprints
- Using anim notifies on animations to run C++ code
- Implemented multiple weapons/weapons types (hit scan, projectile, scatter, explosives) and equipment

*_The topics listed are what I have currently covered as of Jan. 7, 2025_

# Personal Additions
In addition to the topics I learned through the course, I added my own additions to try my hand at applying what I have learned.

Here are a few minor changes I added that were not done as part of the course:
- Displayed player names above their character
- Sensitivity options including aim in sensitivity
- Respawn based on other players distance
- Minor visual changes (spinning bullets, bullets cooling down, etc...)

# Project Showcase

## Multiplayer Plugin
A Game Module plugin that can be used in any project that is meant to be used with Steam. Uses a client-server model with a listen server as host. Includes a simple menu with the ability to host and join active sessions. Client travel map is configurable in Blueprints.

### Functionality
I created a new UGameInstanceSubsystem C++ class, called MultiplayerSessionsSubsystem, which holds the functionality for the custom Online Subsystem class. This class references the World's Online Subsystem its Session Interface to call Session Interface methods for online session functionality.

I created 5 local functions that are used as setup functions for the actual Session Interface functions that are included in the IOnlineSessionPtr interface class (same names but different parameters). In addition to the functions, they each had a delegate and a delegate handle variable that would get added to the session interface's delegate list, which would get triggered after its respective function runs and runs the callback functions bound to them.

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
- Broadcasts to `MultiplayerCreateSessionCompleteDelegate` with _bWasSuccessful_ as parameter.

---

#### FindSessions
Function: `void FindSessions(int32 MaxSearchResults)`
- Adds delegate to delegate list using `SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate)` and save handle in a _FDelegateHandle_.
- Sets up search settings using a `TSharedPtr<FOnlineSessionSearch> LatestSearchSettings` variable.
- Grab local player. Calls `SessionInterface->CreateSessions(UniqueNetId, SearchSettings)`, passing in unique net id from local player, and _LatestSearchSettings_ dereferenced

**Related Delegate and delegate handle:** `FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate` : `FDelegateHandle FindSessionsCompleteDelegateHandle`

**Delegate callback function:** `void OnFindSessionsComplete(bool bWasSuccessful)`
- Clears the delegate from the delegate list using the delegate handle, if Session Interface is valid.
- Broadcasts to `MultiplayerFindSessionsCompleteDelegate`. If any results are found, passes in _SearchResults_ array and _bWasSuccessful_, else, passes in an empty array and false.

---

#### JoinSession
Function: `void JoinSession(const FOnlineSessionSearchResult& SessionResult)`
- Adds delegate to delegate list using `SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate)` and save handle in a _FDelegateHandle_.
- Grab local player. Calls `SessionInterface->CreateSessions(UniqueNetId, SessionName, SessionResult)`, passing in unique net id from local player, the name of the session, and the session itself

**Related Delegate and delegate handle:** `FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate` : `FDelegateHandle JoinSessionCompleteDelegateHandle`

**Delegate callback function:** `void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)`
- Clears the delegate from the delegate list using the delegate handle, if Session Interface is valid.
- Calls `SessionInterface->GetResolvedConnectString(NAME_GameSession, ObtainedAddress)` and stores address info in _ObtainedAddress_.
- Broadcasts to `MultiplayerJoinSessionCompleteDelegate` and passes in _Result_ and _ObtainedAddress_ if connect string was successful.

---

#### DestroySession
Function: `void DestroySession()`
- Adds delegate to delegate list using `SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate)` and save handle in a _FDelegateHandle_.
- Calls `SessionInterface->DestroySession(SessionName)`, passing in NAME_GameSession

**Related Delegate and delegate handle:** `FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate` : `FDelegateHandle DestroySessionCompleteDelegateHandle`

**Delegate callback function:** `void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)`
- Clears the delegate from the delegate list using the delegate handle, if Session Interface is valid.
- Calls `CreateSession(int32 NPC, FString MT)` again and passes in the _NumPublicConnections_ and _MatchType_ variables from CreateSession() that were saved in private member variables.
- Broadcasts to `MultiplayerDestroySessionCompleteDelegate` and passes in _bWasSuccessful_.

---

#### StartSession
Function: `void StartSession()`
- Adds delegate to delegate list using `SessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate)` and save handle in a _FDelegateHandle_.
- Calls `SessionInterface->StartSession(SessionName)`, passing in NAME_GameSession

**Related Delegate and delegate handle:** `FOnStartSessionCompleteDelegate StartSessionCompleteDelegate` : `FDelegateHandle StartSessionCompleteDelegateHandle`

**Delegate callback function:** `void OnStartSessionComplete(FName SessionName, bool bWasSuccessful)`
- Clears the delegate from the delegate list using the delegate handle, if Session Interface is valid.
- Broadcasts to `MultiplayerStartSessionCompleteDelegate` and passes in _bWasSuccessful_.

---
### Menu
A UserWidget class that includes functions for UI setup, button functionality, and similar session functions like in _MultiplayerSessionsSubsystems_ but tailored to calling the Session Intrerface functions and actually moving the player.

To use the functions from the Subsystem, the Menu class had to reference the Subsystem. That way the Menu class can start the Session pipeline and call `CreateSession()` on MultiplayerSessionsSubsystems. However, to continue the pipeline, the Menu class needed to know when its functions would be called in order to do the actual travel. 

This was accomplished by declaring custom delegates on the Subsystem class and binding the Menu class functions to it. This essentially allows the Subsystem class to call Menu functions by broadcasting, avoiding a direct reference to the Menu class. This keeps dependency one-way; Menu depends on Subsystem, but Subsystem does not depend on Menu.

**Custom delegates added to MultiplayerSessionsSubsystems**:
- `FMultiplayerOnCreateSessionComplete MultiplayerCreateSessionCompleteDelegate`: Dynamic Multicast OneParam Delegate
- `FMultiplayerOnFindSessionsComplete MultiplayerFindSessionsCompleteDelegate`: Multicast TwoParam Delegate
- `FMultiplayerOnJoinSessionComplete MultiplayerJoinSessionCompleteDelegate`: Multicast TwoParam Delegate
- `FMultiplayerOnDestroySessionComplete MultiplayerDestroySessionCompleteDelegate`: Dynamic Multicast OneParam Delegate
- `FMultiplayerOnStartSessionComplete MultiplayerStartSessionCompleteDelegate`: Dynamic Multicast OneParam Delegate

**Functions in Menu**:
- `void OnCreateSession(bool bWasSuccessful)`: on success, calls server travel and sends everyone to lobby level.
- `void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)`: loops through SessionResults, checks if Result match type matches _MatchType_ member variable. If so, call `MultiplayerSessionsSubsystems->JoinSession(Result)`
- `void OnJoinSession(EOnJoinSessionCompleteResult::Type Result, const FString& Address)`: if _Result_ is **Success**, gets player controller from game instance and calls `ClientTravel(Address, TRAVEL_Absolute)`
- `void OnDestroySession(bool bWasSuccessful)`: Not used currently (Sep. 8, 2024)
- `void OnStartSession(bool bWasSuccessful)`: Not used currently (Sep. 8, 2024)

The callback functions that will be binded to the dynamic delegates are marked with the `UFUNCTION()` macro

I binded the functions to the delegates in a function called `MenuSetup()` which includes other setup like assigning _MatchType_ and setting focus to the widget.

**Menu Setup**
---
Menu class includes a function that sets up the Menu widget and all functions pertaining to running the Online Subsystem Session Interface code from the widget.

Function: `MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)`
- Defines member variables _MatchType_, _NumPublicConnections_, and _PathToLobby_.
- Adds widget to viewport, sets it as focus and shows mouse cursor using PlayerController
- Gets MultiplayerSessionsSubsystem from game instance
- Binds Menu functions to Subsystem custom delegates using `Delegate.AddDynamic()` for dynamic delegates and `Delegate.AddUObject()` for non-dynamic delegates

Menu class also declares two button widgets that are bounded called **HostButton** and **JoinButton**. These buttons also have callback functions which are bounded to the `OnClicked` event in the `Initialize()` function.

#### Menu Design and Demo
Here is a video of how the menu looks, as well as how hosting and joining sessions looks.

**Menu Design and Hosting a Session**

https://github.com/user-attachments/assets/86407a68-38b3-472c-bf1b-c722b3ff6e69

*_Note: Animations and gameplay are not finished in this clip and is from Sep. 3, 2024_

**Joining Session**

https://github.com/user-attachments/assets/a1305060-0539-45cb-94f1-51a09cb2688f

*_Note: Animations and gameplay are not finished in this clip and is from Sep. 3, 2024_



# Attributions
<a href="https://www.zapsplat.com/music/cyberpunk-electronic-equipment-beep-power-1/" title="death effect sfx">Death effect sound created by ZapSplat</a>

<a href="https://www.flaticon.com/free-icons/grenade" title="grenade icons">Grenade icons created by manshagraphics - Flaticon</a>



