// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode_Multiplayer.generated.h"

/**
 * 
 */
UCLASS()
class TWINSTICKSHOOTER_API AGameMode_Multiplayer : public AGameModeBase
{
	GENERATED_BODY()
	
	AGameMode_Multiplayer();

public:

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	TArray<APlayerController*> ActiveControllers;
};
