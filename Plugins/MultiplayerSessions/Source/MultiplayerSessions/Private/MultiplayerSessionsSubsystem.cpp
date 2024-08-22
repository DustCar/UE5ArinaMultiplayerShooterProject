// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"

#include "OnlineSubsystemUtils.h"
#include "Online/OnlineSessionNames.h"
#include "Engine/LocalPlayer.h"

// Constructor to establish the Session Interface
UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
	}
}

// On Initialize, create the session interface delegates and bind the callback functions to them
void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete);
	FindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete);
	JoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete);
	DestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete);
	StartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete);
}

// Sets up settings for creating a session and calls the Session Interface function CreateSession()
// Different from the SI version of the function
void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	// Searches for an existing session and destroys it to create a new one
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		bCreateSessionOnDestroy = true;
		LatestNumPublicConnections = NumPublicConnections;
		LatestMatchType = MatchType;

		MultiplayerCreateSessionCompleteDelegate.Broadcast(false);
		DestroySession();
		return;
	}

	// Add CreateSessionDelegate to the delegate list to get called after the SI CreateSession() call and store the delegate handle
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// Settings for creating a session
	LatestSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LatestSessionSettings->bIsLANMatch = Online::GetSubsystem(GetWorld())->GetSubsystemName() == "NULL" ? true : false;
	LatestSessionSettings->NumPublicConnections = NumPublicConnections;
	LatestSessionSettings->bAllowJoinInProgress = true;
	LatestSessionSettings->bAllowJoinViaPresence = true;
	LatestSessionSettings->bShouldAdvertise = true;
	LatestSessionSettings->bUsesPresence = true;
	LatestSessionSettings->bUseLobbiesIfAvailable = true;
	LatestSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LatestSessionSettings->BuildUniqueId = 1;

	// Obtain LocalPlayer to get a unique net id to pass in for CreateSession()
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	// Check if CreateSession() fails
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LatestSessionSettings))
	{
		// remove CreateSessionDelegate from delegate list
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		// Broadcast to custom CreateSessionCompleteDelegate for menu class
		MultiplayerCreateSessionCompleteDelegate.Broadcast(false);
	}
}

// Sets up settings for finding a session and calls the Session Interface function FindSessions()
// Different from the SI version of the function
void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	// Add FindSessionDelegate to delegate list and gets called after SI FindSessions() is called, store the delegate handle
	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	// Settings for finding sessions
	LatestSearchSettings = MakeShareable(new FOnlineSessionSearch());
	LatestSearchSettings->MaxSearchResults = MaxSearchResults;
	LatestSearchSettings->bIsLanQuery = Online::GetSubsystem(GetWorld())->GetSubsystemName() == "NULL" ? true : false;
	LatestSearchSettings->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// Obtain LocalPlayer to get a unique net id to pass in for FindSessions()
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	// Check if FindSessions() fails
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LatestSearchSettings.ToSharedRef()))
	{
		// remove FindSessionsDelegate from delegate list
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

		// Broadcast to custom FindSessionsCompleteDelegate an empty string after failure
		MultiplayerFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

// Sets up delegates for JoinSession() Session Interface function and runs it
// Different from the SI version of the function
void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!SessionInterface.IsValid())
	{
		// Broadcast to menu class JoinSessionDelegate that an error occurred and pass an empty string
		MultiplayerJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError, FString());
		return;
	}

	// Add JoinSessionDelegate to delegate list and gets called after SI JoinSession() is called, store handle
	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	// Obtain LocalPlayer to get a unique net id to pass in for JoinSession()
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	// Check if JoinSession() fails
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		// Remove delegate from delegate list
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		// Broadcast to menu class JoinSessionDelegate that an error occurred and pass an empty string
		MultiplayerJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError, FString());
	}
}

// Sets up delegates for DestroySession() Session Interface function and runs it
// Different from the SI version of the function
void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!SessionInterface.IsValid())
	{
		// Broadcast to custom DestroySessionDelegate that it failed
		MultiplayerDestroySessionCompleteDelegate.Broadcast(false);
		return;
	}

	// Add DestroySessionDelegate to delegate list and gets called after SI DestroySession() is called
	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	// Checks if DestroySession() fails
	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		// Remove delegate from delegate list
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);

		// Broadcast to custom DestroySessionDelegate that it failed
		MultiplayerDestroySessionCompleteDelegate.Broadcast(false);
	}
	
}

// Sets up delegates for DestroySession() Session Interface function and runs it
// Different from the SI version of the function
void UMultiplayerSessionsSubsystem::StartSession()
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	// Add StartSessionDelegate to delegate list and gets called after SI StartSession() is called
	StartSessionCompleteDelegateHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate);

	// Checks if StartSession() fails
	if (!SessionInterface->StartSession(NAME_GameSession))
	{
		// Remove delegate from delegate list
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);

		// Broadcast to custom StartSessionDelegate that it failed
		MultiplayerStartSessionCompleteDelegate.Broadcast(false);
	}
}

// Callback function to get called after SI CreateSession() called
void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// Check if Session Interface value is valid and remove delegate from list since callback is reached already
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	// Broadcast to custom delegate to Menu class
	MultiplayerCreateSessionCompleteDelegate.Broadcast(bWasSuccessful);
}

// Callback function to get called after SI FindSessions() called
void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	// Check if Session Interface value is valid and remove delegate from list since callback is reached already
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	// Check if search results have no results returned and broadcast to custom delegate to Menu class an empty string
	if (LatestSearchSettings->SearchResults.Num() <= 0)
	{
		MultiplayerFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	// Broadcast to custom delegate and pass in search results from FindSessions() online search settings
	MultiplayerFindSessionsCompleteDelegate.Broadcast(LatestSearchSettings->SearchResults, bWasSuccessful);
}

// Callback function to get called after SI JoinSession() called
void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	// Check if Session Interface value is valid and remove delegate from list since callback is reached already
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	// Create a local FString variable to hold address info of session
	FString ObtainedAddress;
	// Call GetResolvedConnectString() to obtain connection info and save into ObtainedAddress
	if (SessionInterface->GetResolvedConnectString(NAME_GameSession, ObtainedAddress))
	{
		// Broadcast to custom delegate and pass in Result type and address info for the class callback funciton to use
		MultiplayerJoinSessionCompleteDelegate.Broadcast(Result, ObtainedAddress);
	}
}

// Callback function to get called after SI DestroySession() called
void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	// Check if Session Interface value is valid and remove delegate from list since callback is reached already
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	// Check if SI DestroySession() call succeeded and if CreateSessionOnDestroy is true
	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		// Reset boolean to false
		bCreateSessionOnDestroy = false;

		// Create a new session using our custom CreateSession and the previously used parameters
		CreateSession(LatestNumPublicConnections, LatestMatchType);
	}

	// Broadcast to custom delegate
	MultiplayerDestroySessionCompleteDelegate.Broadcast(bWasSuccessful);
}

// Callback function to get called after SI CreateSession() called
void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// Check if Session Interface value is valid and remove delegate from list since callback is reached already
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
	}

	// Broadcast to custom delegate
	MultiplayerStartSessionCompleteDelegate.Broadcast(bWasSuccessful);
}
