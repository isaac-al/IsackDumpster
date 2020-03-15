// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerController_Multiplayer.generated.h"

/**
 * 
 */
UCLASS()
class TWINSTICKSHOOTER_API APlayerController_Multiplayer : public APlayerController
{
	GENERATED_BODY()
	
public:

	virtual void Tick( float DeltaSeconds ) override;

};
