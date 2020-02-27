// Fill out your copyright notice in the Description page of Project Settings.

#include "SwarmGameWave.h"
#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Runtime/Engine/Public/EngineGlobals.h"


int32 FSpawnerData::ID = 0;

// Sets default values
ASwarmGameWave::ASwarmGameWave()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//++ASwarmGameWave::ID;

}

// Called when the game starts or when spawned
void ASwarmGameWave::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASwarmGameWave::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsFinished())
	{
		OnWaveEnd.Broadcast();
	}
}

void ASwarmGameWave::AddSpawner(FSpawnerData pData)
{
	auto tLocation = pData.GetLocations();
	auto tAmount = pData.GetAmounts();
	auto tType = pData.GetTypes();

	for (int i = 0; i < pData.GetMax(); ++i)
	{
		AEnemySpawner* tSpawner = GetWorld()->SpawnActor<AEnemySpawner>();
		tSpawner->InitSpawner(tType[i], tLocation[i], tAmount[i]);
		SpawnerArray.Add(tSpawner);
	}
}

void ASwarmGameWave::InitSpawners() const
{
	for (auto& tS : SpawnerArray)
	{
		tS->SpawnWave(tS->EnemyType, tS->SpawnAmount, tS->ScatterWeight, 
			tS->WanderWeight, tS->SeekWeight, tS->SeparateWeight, 
			tS->AlignWeight, tS->CohereWeight);
	}
}

bool ASwarmGameWave::IsFinished() const
{
	for (auto& tSpawner : SpawnerArray)
	{
		if (!tSpawner->IsFinished())
			return false;
	}

	return true;
}


