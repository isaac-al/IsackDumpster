// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SwarmGameMode.generated.h"

/**
 * 
 */

class ASwarmGameWave;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameWon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FCountdown );

UCLASS(Blueprintable)
class TWINSTICKSHOOTER_API ASwarmGameMode : public AGameModeBase
{
	GENERATED_BODY()
protected:

	UFUNCTION()
	virtual void BeginPlay() override;
	void Tick(float DeltaTime);

	virtual void GenericPlayerInitialization(AController* C) override;

public:
	ASwarmGameMode();
};
