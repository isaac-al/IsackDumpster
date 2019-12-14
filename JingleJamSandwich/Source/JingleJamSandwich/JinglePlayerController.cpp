// Fill out your copyright notice in the Description page of Project Settings.


#include "JinglePlayerController.h"
#include "JingleJamSandwichGameModeBase.h"
#include <Kismet/GameplayStatics.h>
#include <WidgetBlueprintLibrary.h>
#include <UserWidget.h>

void AJinglePlayerController::BeginPlay()
{
	FInputModeGameAndUI input;
	input.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	bShowMouseCursor = true;
	SetInputMode(input);
}

void AJinglePlayerController::SetupInputComponent()
{
	if (!InputComponent)
	{
		Super::SetupInputComponent();
 	}
	
	InputComponent->BindAction("Action", EInputEvent::IE_Released, this, &AJinglePlayerController::ActionReleased);
	InputComponent->BindAction("Drop", EInputEvent::IE_Released, this, &AJinglePlayerController::DropReleased);
	InputComponent->BindAction("Pause", EInputEvent::IE_Released, this, &AJinglePlayerController::PauseReleased);
}

void AJinglePlayerController::ActionReleased()
{
	AJingleJamSandwichGameModeBase* gamemode = Cast<AJingleJamSandwichGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gamemode && gamemode->GameState == ePlaying)
	{
		gamemode->GameState = eLost;
		gamemode->bPleaseOpenGameOverThanks = true;
	}
}

void AJinglePlayerController::DropReleased()
{
	AJingleJamSandwichGameModeBase* gamemode = Cast<AJingleJamSandwichGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gamemode)
	{
		if (gamemode->GameState == eMainMenu)
		{
			gamemode->GameState = ePlaying;
		}
		else if (gamemode->GameState == eWon || gamemode->GameState == eLost)
		{
			gamemode->GameState = eMainMenu;
		}
		
	}
}

void AJinglePlayerController::PauseReleased()
{
	AJingleJamSandwichGameModeBase* gamemode = Cast<AJingleJamSandwichGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gamemode)
	{
		if (gamemode->GameState != ePaused && gamemode->GameState == ePlaying)
		{
			gamemode->GameState = ePaused;
			gamemode->bPleaseOpenPauseThanks = true;
		}
		else if (gamemode->GameState == ePaused)
		{
			gamemode->GameState = ePlaying;
			gamemode->bPleaseOpenPauseThanks = false;
		}

	}
}
