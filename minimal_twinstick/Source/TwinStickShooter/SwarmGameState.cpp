// Fill out your copyright notice in the Description page of Project Settings.

#include "SwarmGameState.h"
#include "SwarmGameWave.h"


ASwarmGameState::ASwarmGameState() : CurrentWave(-1), TotalTime(0.0f)
{

}

// Called every frame
void ASwarmGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TotalTime += DeltaTime;
}


