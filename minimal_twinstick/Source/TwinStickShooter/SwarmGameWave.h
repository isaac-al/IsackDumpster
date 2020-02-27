// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.h"
#include "SwarmGameWave.generated.h"


USTRUCT()
struct TWINSTICKSHOOTER_API FSpawnerData
{
	GENERATED_BODY()
private:
	TArray<FVector> SpawnerLocation;
	TArray<int32> SpawnAmount;
	TArray<EEnemyType> EnemyType;

	int32 Max = 0;
	int32 WaveId = 0;
	
	static int32 ID;

public:

	FSpawnerData() { WaveId = ++ID; }

	void AddData(EEnemyType pType, FVector pLocation, int32 pAmount)
	{
		Max++;
		SpawnerLocation.Add(pLocation);
		SpawnAmount.Add(pAmount);
		EnemyType.Add(pType);
	}

	TArray<FVector> GetLocations() { return SpawnerLocation; }
	TArray<int32> GetAmounts() { return SpawnAmount; }
	TArray<EEnemyType> GetTypes() { return EnemyType; }
	int32 GetMax() { return Max; }
	int32 GetWaveId() { return WaveId; }

	/*Enemy Type (Add later)*/
};

//int32 FSpawnerData::WaveCount = 0;


class AEnemySpawner;

DECLARE_MULTICAST_DELEGATE( FWaveEnd );


UCLASS()
class TWINSTICKSHOOTER_API ASwarmGameWave : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwarmGameWave();


	//static int ID;

protected:


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	FSpawnerData SpawnerData;

	UPROPERTY()
	TArray<AEnemySpawner*> SpawnerArray;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void AddSpawner(FSpawnerData pData);
	
	UFUNCTION()
	void InitSpawners() const;

	UFUNCTION()
	bool IsFinished() const;

	FWaveEnd OnWaveEnd;
	
};

//int ASwarmGameWave::ID = 0;
