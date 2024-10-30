// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArinaAnnouncement.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class ARINA_API UArinaAnnouncement : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AnnouncementText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AnnouncementTimer;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoText;
};
