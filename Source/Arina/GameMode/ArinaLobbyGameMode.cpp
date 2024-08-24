// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaLobbyGameMode.h"

#include "GameFramework/GameStateBase.h"

void AArinaLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if (NumberOfPlayers == 2)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(FString("/Game/Maps/ArinaMap?listen"), true);
		}
	}
}
