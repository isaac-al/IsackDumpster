// Fill out your copyright notice in the Description page of Project Settings.


#include "JingleJamSandwichGameModeBase.h"
#include <Engine/Engine.h>
#include "JinglePlayerController.h"
#include "Toy/Toy.h"
#include <Kismet/GameplayStatics.h>
#include "Blueprint/UserWidget.h"
#include <WidgetBlueprintLibrary.h>
#include <GameFramework/PlayerController.h>

AJingleJamSandwichGameModeBase::AJingleJamSandwichGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerControllerClass = AJinglePlayerController::StaticClass();
}

void AJingleJamSandwichGameModeBase::StartGame()
{
	// iterate through players
	TArray<AActor*> controllers;
	TSubclassOf<AJinglePlayerController> classs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), classs, controllers);

	for (int32 i = 0; i < controllers.Num(); i++)
	{
		AJinglePlayerController* pc = Cast<AJinglePlayerController>(controllers[i]);

		if (pc && pc->IsValidLowLevel())
		{
			FInputModeGameAndUI input;
			input.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			pc->bShowMouseCursor = false;
			pc->SetInputMode(input);
		}
	}
	GameState = ePlaying;
	SpawnToy();

	bPleaseOpenMainThanks = false;
	bPleaseOpenGameOverThanks = false;
	// TODO: Display HUD
}

void AJingleJamSandwichGameModeBase::Pause()
{
	TArray<AActor*> controllers;
	TSubclassOf<AJinglePlayerController> classs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), classs, controllers);

	for (int32 i = 0; i < controllers.Num(); i++)
	{
		AJinglePlayerController* pc = Cast<AJinglePlayerController>(controllers[i]);

		if (pc && pc->IsValidLowLevel())
		{
			FInputModeGameAndUI input;
			input.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			pc->bShowMouseCursor = true;
			pc->SetInputMode(input);
		}
	}

	bPleaseOpenPauseThanks = true;
	GameState = ePaused;
}

void AJingleJamSandwichGameModeBase::Resume()
{
	TArray<AActor*> controllers;
	TSubclassOf<AJinglePlayerController> classs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), classs, controllers);

	for (int32 i = 0; i < controllers.Num(); i++)
	{
		AJinglePlayerController* pc = Cast<AJinglePlayerController>(controllers[i]);

		if (pc && pc->IsValidLowLevel())
		{
			FInputModeGameAndUI input;
			input.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			pc->bShowMouseCursor = true;
			pc->SetInputMode(input);
		}
	}
	bPleaseOpenPauseThanks = false;
	GameState = ePlaying;
}

void AJingleJamSandwichGameModeBase::BackToMenu()
{
	// iterate through players
	TArray<AActor*> controllers;
	TSubclassOf<AJinglePlayerController> classs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), classs, controllers);

	for (int32 i = 0; i < controllers.Num(); i++)
	{
		AJinglePlayerController* pc = Cast<AJinglePlayerController>(controllers[i]);

		if (pc && pc->IsValidLowLevel())
		{
			FInputModeGameAndUI input;
			input.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			pc->bShowMouseCursor = true;
			pc->SetInputMode(input);
		}
	}	
	bPleaseOpenPauseThanks = false;
	bPleaseOpenGameOverThanks = false;
	bPleaseOpenMainThanks = true;
	GameState = eMainMenu;
}

void AJingleJamSandwichGameModeBase::Restart()
{
	Reset();
	bPleaseOpenPauseThanks = false;
	bPleaseOpenGameOverThanks = false;
	GameState = ePlaying;
}

void AJingleJamSandwichGameModeBase::SpawnToy()
{
	GetWorld()->SpawnActor<AToy>(AToy::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
}

void AJingleJamSandwichGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	bPleaseOpenMainThanks = true;
	bPleaseOpenPauseThanks = false;
	bPleaseOpenGameOverThanks = false;
	Reset();
}

void AJingleJamSandwichGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DeltaTime = DeltaSeconds;

	switch (GameState)
	{
	case eMainMenu:
	{
		UpdateMainMenu();
	} break;
	case ePlaying:
	{
		UpdateGame();
	} break;
	case ePaused:
	{
		UpdatePauseMenu();
	} break;

	case eWon:
	{
		UpdateWonState();
	} break;

	case eLost:
	{
		UpdateLossState();
	} break;
	default:
		break;
	}
}

void AJingleJamSandwichGameModeBase::UpdateMainMenu()
{
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString("MAIN MENU"));

}

void AJingleJamSandwichGameModeBase::UpdatePauseMenu()
{
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString("PAUSED"));
}

void AJingleJamSandwichGameModeBase::UpdateWonState()
{
	Reset();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString("You wonned"));
}

void AJingleJamSandwichGameModeBase::UpdateLossState()
{
	Reset();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString("YOU LOST MEGABITCH"));
}

void AJingleJamSandwichGameModeBase::UpdateGame()
{
	GameTimer -= DeltaTime;

	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString("Time left: " + FString::SanitizeFloat(GameTimer)));

	if (GameTimer <= 0.0f)
	{
		// Game over biiiitch
		GameState = eWon;
		bPleaseOpenGameOverThanks = true;
	}
}

void AJingleJamSandwichGameModeBase::Reset()
{
	GameTimer = GAME_TIME;
}
