// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController_Multiplayer.h"
#include "TwinStickGameInstance.h"
#include "Kismet/GameplayStatics.h"

float server_timer = 0;
bool joined = false;

void APlayerController_Multiplayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
