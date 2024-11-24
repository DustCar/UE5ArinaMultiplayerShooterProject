// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArinaSniperScopeWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class ARINA_API UArinaSniperScopeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UImage* ScopeOverlay;

	UPROPERTY(meta = (BindWidget))
	UImage* Background;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ZoomInAnim;
};
