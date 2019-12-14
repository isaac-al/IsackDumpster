// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "JingleJamSandwichGameModeBase.generated.h"

#define GAME_TIME 10.0f

enum EState
{
	eMainMenu,
	ePlaying,
	ePaused,
	eWon,
	eLost
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
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine")
	EState GameState;
	
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

	UPROPERTY(BlueprintReadWrite)
	bool bPleaseOpenPauseThanks = false;

	UPROPERTY(BlueprintReadWrite)
	bool bPleaseOpenMainThanks = false;

	UPROPERTY(BlueprintReadWrite)
	bool bPleaseOpenGameOverThanks = false;
	
private:
	void UpdateMainMenu();
	void UpdatePauseMenu();
	void UpdateWonState();
	void UpdateLossState();
	void UpdateGame();
	void Reset();
};
