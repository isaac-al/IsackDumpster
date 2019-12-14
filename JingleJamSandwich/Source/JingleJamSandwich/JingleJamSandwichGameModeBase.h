// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "JingleJamSandwichGameModeBase.generated.h"

class AToy;
class AElf;
class AKrampus;

#define GAME_TIME 200.0f
#define ELF_HEALTH_MAX 3
#define ELF_LIVES_MAX 3
#define NUM_MACHINES 4
#define DELIVER_POINTS 50
#define LIST_CLEAR_POINTS 100
#define MAX_REPAIR_TIME 5.0f

enum EState
{
	eMainMenu,
	ePlaying,
	ePaused,
	eWon,
	eLost,
	eStateMax
};

enum EMachineColour
{
	eRed,
	eBlue,
	eGreen,
	eYellow,
	eColourMax
};

struct FToyItem
{
	int32 ItemType = -1;
	EMachineColour colour = eColourMax;
};

struct FMachine
{
	EMachineColour Colour;
	float RepairTime = 0.0f;
	bool Broken = false;
};

/**
 * 
 */
UCLASS(Blueprintable)
class JINGLEJAMSANDWICH_API AJingleJamSandwichGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:

	AJingleJamSandwichGameModeBase();

	AElf* Elf; 
	AKrampus* Krampus;

	EState GameState;
	TArray<FToyItem> ItemList;
	
	UPROPERTY(BlueprintReadOnly)
	int32 ElfHealth = ELF_HEALTH_MAX;

	UPROPERTY(BlueprintReadOnly)
	int32 ElfLives = ELF_LIVES_MAX;

	UPROPERTY(BlueprintReadOnly)
	float ElfHealthNormal = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float GameTimer = GAME_TIME;

	UPROPERTY(BlueprintReadOnly)
	int32 Score = 0;

	FMachine Machines[NUM_MACHINES];
	float DeltaTime = 0.0f;

	FVector ElfStart = FVector(-1900.0f, 1540.0f, -9.999786f);

	UFUNCTION(BlueprintCallable)
	void StartGame();

	UFUNCTION(BlueprintCallable)
	void Pause();

	UFUNCTION(BlueprintCallable)
	void Resume();

	UFUNCTION(BlueprintCallable)
	void BackToMenu();

	UFUNCTION(BlueprintCallable)
	void Restart();

	UFUNCTION(BlueprintCallable)
	void SpawnToy();

	void DamageElf();
	void MakeList();
	void PaintToy(AToy* InToy, EMachineColour InColour);
	void DeliverToy(AToy* InToy);
	void DestroyToy(AToy* InToy);

	void BreakMachine(EMachineColour InMachineColour);
	void RepairMachine(EMachineColour InMachineColour);


	void ChangeMaterial(FString MaterialName, AToy* InToy);

	UPROPERTY(BlueprintReadWrite)
	bool bPleaseOpenPauseThanks = false;
	UPROPERTY(BlueprintReadWrite)
	bool bPleaseOpenMainThanks = false;

	UPROPERTY(BlueprintReadWrite)
	bool bPleaseOpenGameOverThanks = false;

	UPROPERTY(BlueprintReadWrite)
	bool bShowHUD = false;

private:

	void UpdateMainMenu();
	void UpdatePauseMenu();
	void UpdateWonState();
	void UpdateLossState();
	void UpdateGame();
	void Reset();
	void KillElf();
};
