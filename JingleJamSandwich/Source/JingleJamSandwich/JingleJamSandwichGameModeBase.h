// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "JingleJamSandwichGameModeBase.generated.h"

class AToy;

#define GAME_TIME 200.0f
#define ELF_HEALTH_MAX 3
#define ELF_LIVES_MAX 3
#define NUM_MACHINES 4

struct FToyItem
{
	int32 ItemType = -1;
	EMachineColour colour = eColourMax;
}

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

	EState GameState;

	TArray<FToyItem> ItemList;
	
	UPROPERTY(BlueprintReadOnly)
	int32 ElfHealth = ELF_HEALTH_MAX;

	UPROPERTY(BlueprintReadOnly)
	int32 ElfLives = ELF_LIVES_MAX;

	FMachine Machines[NUM_MACHINES];
	float DeltaTime = 0.0f;
	float GameTimer = GAME_TIME;

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
