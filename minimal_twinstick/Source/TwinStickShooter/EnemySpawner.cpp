// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Runtime/Engine/Public/EngineGlobals.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "EngineUtils.h"
#include "Enemy.h"


AEnemySpawner::AEnemySpawner()
{
	TArray<AEnemy*> SpawnedEnemies = TArray<AEnemy*>();

	//SetActorTickEnabled(true); 
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bHidden = false;
}


void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemySpawner::SpawnWave(EEnemyType pType, float pAmount, float pSctr, float pWndr, float pSeek, float pSep, float pAlign, float pCoh)
{
	for (int i = 0; i < SpawnAmount; ++i)
	{
		FActorSpawnParameters tParams = FActorSpawnParameters();
		tParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FVector tRand = FMath::VRand();
		FVector tLoc = GetActorLocation() + FVector(FMath::FRandRange(-SpawnRadius, SpawnRadius), FMath::FRandRange(-SpawnRadius, SpawnRadius), 0);
		FRotator tRot = FRotator::ZeroRotator;

		auto tEnemy = GetWorld()->SpawnActor<AEnemy>(tLoc, tRot, tParams);
		tEnemy->Init(pType,pSctr, pWndr, pSeek, pSep, pAlign, pCoh);

		SpawnedEnemies.Add(tEnemy);
	}

}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bool tSpawn = true;

	for (auto t : SpawnedEnemies)
	{
		if (IsValid(t))
			tSpawn = false;
	}

	if (!tSpawn)
	{

		//UE_LOG(LogTemp, Warning, TEXT("SPAWNING"));
		//SpawnWave(5, ScatterWeight, WanderWeight, SeekWeight,
		//	SeparateWeight, AlignWeight, CohereWeight);
	}


}

void AEnemySpawner::SpawnWaveDefault()
{
	
	SpawnWave(EEnemyType::ET_Basic, SpawnAmount, ScatterWeight, WanderWeight, SeekWeight,
		SeparateWeight, AlignWeight, CohereWeight);
}

void AEnemySpawner::InitSpawner(EEnemyType pType, FVector pLocation, int32 pAmount)
{
	this->SetActorLocation(pLocation);
	SpawnAmount = pAmount;
	EnemyType = pType;
	UE_LOG(LogTemp, Warning, TEXT("SPAWNING"));

	
}

bool AEnemySpawner::IsFinished() const
{
	for (auto& enemy : SpawnedEnemies)
	{
		if (IsValid(enemy))
		{
			return false;
		}
	}

	return true;
}


