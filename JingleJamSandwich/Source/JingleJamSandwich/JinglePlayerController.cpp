// Fill out your copyright notice in the Description page of Project Settings.


#include "JinglePlayerController.h"
#include "JingleJamSandwichGameModeBase.h"
#include <Kismet/GameplayStatics.h>
#include <WidgetBlueprintLibrary.h>
#include <UserWidget.h>
#include <JingleJamSandwich\Pawns\Krampus.h>
#include <JingleJamSandwich\Pawns\Elf.h>

void AJinglePlayerController::BeginPlay()
{
	FInputModeGameAndUI input;
	input.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	bShowMouseCursor = true;
	SetInputMode(input);

	TArray<AActor*> Krampai;
	TSubclassOf<AKrampus> krampus;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AKrampus::StaticClass(), Krampai);
	Krampus = Cast<AKrampus>(Krampai[0]);

	TArray<AActor*> Elves;
	TSubclassOf<AElf> elf;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AElf::StaticClass(), Elves);
	Elf = Cast<AElf>(Elves[0]);
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
	//called before begin play.
	InputComponent->BindAxis("Krampus_X", this, &AJinglePlayerController::KrampusMoveX);
	InputComponent->BindAxis("Krampus_Y", this, &AJinglePlayerController::KrampusMoveY);
	InputComponent->BindAxis("Elf_X", this, &AJinglePlayerController::ElfMoveX);
	InputComponent->BindAxis("Elf_Y", this, &AJinglePlayerController::ElfMoveY);
}

void AJinglePlayerController::ActionReleased()
{
	AJingleJamSandwichGameModeBase* gamemode = Cast<AJingleJamSandwichGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gamemode && gamemode->GameState == ePlaying)
	{
		gamemode->DamageElf();
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
		else if (gamemode->GameState == ePlaying)
		{
			gamemode->MakeList();
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

void AJinglePlayerController::KrampusMoveX(float amount)
{
	Krampus->MoveX(amount);
}

void AJinglePlayerController::KrampusMoveY(float amount)
{
	Krampus->MoveY(amount);
}

void AJinglePlayerController::ElfMoveX(float amount)
{
	Elf->MoveX(amount);
}

void AJinglePlayerController::ElfMoveY(float amount)
{
	Elf->MoveY(amount);
}
