// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "EnemySpawner.generated.h"

/**
 * 
 */

class AEnemy;

DECLARE_MULTICAST_DELEGATE( FEnemiesDead );


UCLASS()
class TWINSTICKSHOOTER_API AEnemySpawner : public ATargetPoint
{
	GENERATED_BODY()

protected:

	AEnemySpawner();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	float SpawnRadius = 300.0f;
public:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int SpawnAmount = 1;

	UPROPERTY()
	EEnemyType EnemyType;

	UPROPERTY(Category = "Weights", EditAnywhere, meta = (AllowPrivateAccess = "true"))
		float ScatterWeight = 0.5f;

	UPROPERTY(Category = "Weights", EditAnywhere, meta = (AllowPrivateAccess = "true"))
		float WanderWeight = 0.5f;

	UPROPERTY(Category = "Weights", EditAnywhere, meta = (AllowPrivateAccess = "true"))
		float SeekWeight = 5.0f;

	UPROPERTY(Category = "Weights", EditAnywhere, meta = (AllowPrivateAccess = "true"))
		float SeparateWeight = 6.0f;

	UPROPERTY(Category = "Weights", EditAnywhere, meta = (AllowPrivateAccess = "true"))
		float AlignWeight = 1.0f;

	UPROPERTY(Category = "Weights", EditAnywhere, meta = (AllowPrivateAccess = "true"))
		float CohereWeight = 3.0f;

	UPROPERTY()
	TArray<AEnemy*> SpawnedEnemies;

	

	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void InitSpawner(EEnemyType pType, FVector pLocation, int32 pAmount);

	UFUNCTION()
	bool IsFinished() const;

	UFUNCTION()
	void SpawnWave(EEnemyType pType, float pAmount, float pSctr, float pWndr, float pSeek, float pSep, float pAlign, float pCoh);

	UFUNCTION()
	void SpawnWaveDefault();

	FEnemiesDead OnEnemiesDead;
};
