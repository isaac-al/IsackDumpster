// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "JinglePlayerController.generated.h"

class AKrampus;
class AElf;

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
	void ActionPressed();
	void ActionReleased();
	void DropReleased();
	void PauseReleased();

	void KrampusActionReleased();

	void KrampusMoveX(float amount);
	void KrampusMoveY(float amount);
	void ElfMoveX(float amount);
	void ElfMoveY(float amount);

	AKrampus* Krampus = nullptr;
	AElf* Elf = nullptr;
};
