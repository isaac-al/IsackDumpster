// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SwarmGameState.generated.h"

/**
 * 
 */

UCLASS()
class TWINSTICKSHOOTER_API ASwarmGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:
	ASwarmGameState();	

	UPROPERTY(BlueprintReadOnly)
	float TotalTime;

	UFUNCTION()
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentWave;
	
};
