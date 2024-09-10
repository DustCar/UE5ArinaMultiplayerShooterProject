// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaOverHeadWidget.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UArinaOverHeadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UArinaOverHeadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString PlayerRole;

	switch (RemoteRole)
	{
	case ROLE_Authority:
		PlayerRole = FString("Authority");
		break;
	case ROLE_AutonomousProxy:
		PlayerRole = FString("AutonomousProxy");
		 break;
	case ROLE_SimulatedProxy:
		PlayerRole = FString("SimulatedProxy");
		break;
	case ROLE_None:
		PlayerRole = FString("None");
		break;
	default:
		PlayerRole = FString("None");
		break;
	}

	APlayerState* PlayerState = InPawn->GetPlayerState();
	FString PlayerName = FString(TEXT("Unnamed"));

	if (PlayerState)
	{
		PlayerName = PlayerState->GetPlayerName();
	}

	FString RoleSentence = FString::Printf(TEXT("Remote role: %s - %s"), *PlayerRole, *PlayerName);
	SetDisplayText(RoleSentence);
}

void UArinaOverHeadWidget::ShowPlayerName(APawn* InPawn)
{
	APlayerState* PlayerState = InPawn->GetPlayerState();
	if (PlayerState != nullptr)
	{
		FString PlayerName = FString::Printf(TEXT("%s"), *PlayerState->GetPlayerName());
		SetDisplayText(PlayerName);
		
	}
}

void UArinaOverHeadWidget::NativeDestruct()
{
	RemoveFromParent();
	
	Super::NativeDestruct();
}
