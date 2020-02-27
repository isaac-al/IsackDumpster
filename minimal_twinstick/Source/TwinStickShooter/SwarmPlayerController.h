// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SwarmPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TWINSTICKSHOOTER_API ASwarmPlayerController : public APlayerController
{
	GENERATED_BODY()

	ASwarmPlayerController();



		// TODO: Store a copy of HUDActor to send message from pawn, through this class to HUDActor that player is dead
protected:

	virtual void BeginPlay() override;


	UPROPERTY()
	TSubclassOf<AActor> HudActorClass;
};
