// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "JinglePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class JINGLEJAMSANDWICH_API AJinglePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// input 
	void ActionReleased();
	void DropReleased();
	void PauseReleased();
};
