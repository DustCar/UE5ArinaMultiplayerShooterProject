// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArinaOverHeadWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARINA_API UArinaOverHeadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

	void SetDisplayText(FString TextToDisplay);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);
	
	UFUNCTION(BlueprintCallable)
	void ShowPlayerName(APawn* InPawn);
	
protected:
	virtual void NativeDestruct() override;

	FTimerHandle PlayerNameTimerHandle;

};
