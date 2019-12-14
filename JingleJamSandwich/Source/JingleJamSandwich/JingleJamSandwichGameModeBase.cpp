// Fill out your copyright notice in the Description page of Project Settings.


#include "JingleJamSandwichGameModeBase.h"
#include <Engine/Engine.h>
#include "JinglePlayerController.h"
#include "Toy/Toy.h"
#include "Pawns/Elf.h"
#include <Kismet/GameplayStatics.h>
#include "Blueprint/UserWidget.h"
#include <WidgetBlueprintLibrary.h>
#include <GameFramework/PlayerController.h>
#include <JingleJamSandwich\Pawns\Elf.h>
#include <JingleJamSandwich\Pawns\Krampus.h>

AJingleJamSandwichGameModeBase::AJingleJamSandwichGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerControllerClass = AJinglePlayerController::StaticClass();
}

void AJingleJamSandwichGameModeBase::StartGame()
{
	// iterate through players
	TArray<AActor*> controllers;
	TArray<AActor*> elves;
	TArray<AActor*> krampai;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AJinglePlayerController::StaticClass(), controllers);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AElf::StaticClass(), elves);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AKrampus::StaticClass(), krampai);

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

	Elf = Cast<AElf>(elves[0]);
	Krampus = Cast<AKrampus>(krampai[0]);

	AJinglePlayerController* elfPC = Cast<AJinglePlayerController>(controllers[0]);

	elfPC->Possess(Elf);
	SpawnToy();

	bPleaseOpenMainThanks = false;
	bPleaseOpenGameOverThanks = false;
	// TODO: Display HUD
	Reset();
}

void AJingleJamSandwichGameModeBase::Pause()
{
	TArray<AActor*> controllers;
	TSubclassOf<AJinglePlayerController> jingleController;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), jingleController, controllers);

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
	TSubclassOf<AJinglePlayerController> jingleController;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), jingleController, controllers);

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
	TSubclassOf<AJinglePlayerController> jingleController;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), jingleController, controllers);

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

void AJingleJamSandwichGameModeBase::DamageElf()
{
	--ElfHealth;

	if (ElfHealth <= 0)
	{
		KillElf();
	}
	else
	{
	}	
}

void AJingleJamSandwichGameModeBase::MakeList()
{
	ItemList.Empty();

	for (int32 i = 0; i < 4; ++i)
	{
		FToyItem item;
		item.ItemType = FMath::RandRange(0, 5);
		item.colour = (EMachineColour)FMath::RandRange(0, 4);
		ItemList.Add(item);
	}
}

void AJingleJamSandwichGameModeBase::PaintToy(AToy* InToy, EMachineColour InColour)
{
	// TODO: set mesh colour
	if (InToy && InToy->IsValidLowLevel())
	{
		InToy->colour = InColour;
	}
}

void AJingleJamSandwichGameModeBase::DeliverToy(AToy* InToy)
{
	bool validToy = false;
	if (!InToy) return;
	int32 itemType = InToy->itemType;
	EMachineColour toyColour = InToy->colour;

	for (int32 i = 0; i < ItemList.Num(); ++i)
	{
		if (itemType == ItemList[i].ItemType &&
			toyColour == ItemList[i].colour)
		{
			ItemList.RemoveAt(i);
			break;
		}
	}

	if (!validToy)
		return;

	InToy->BeginDestroy();
}

void AJingleJamSandwichGameModeBase::DestroyToy(AToy* InToy)
{
	if (InToy && InToy->IsValidLowLevel())
	{
		InToy->BeginDestroy();
	}
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
	bShowHUD = false;
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString("MAIN MENU"));

}

void AJingleJamSandwichGameModeBase::UpdatePauseMenu()
{
	bShowHUD = false;
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString("PAUSED"));
}

void AJingleJamSandwichGameModeBase::UpdateWonState()
{
	bShowHUD = false;
	bPleaseOpenGameOverThanks = true;

	Reset();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString("You wonned"));
}

void AJingleJamSandwichGameModeBase::UpdateLossState()
{
	bShowHUD = false;
	bPleaseOpenGameOverThanks = true;
	Reset();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString("YOU LOST MEGABITCH"));
}

void AJingleJamSandwichGameModeBase::UpdateGame()
{
	GameTimer -= DeltaTime;

	bShowHUD = true;
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString("Time left: " + FString::SanitizeFloat(GameTimer)));
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString("Health left: " + FString::FromInt(ElfHealth)));

	if (GameTimer <= 0.0f)
	{
		// Game over biiiitch
		GameState = eWon;
		bPleaseOpenGameOverThanks = true;
	}

	for (int item = 0; item < ItemList.Num(); ++item)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString("Item: " + FString::FromInt(ItemList[item].ItemType)));
	}
}

void AJingleJamSandwichGameModeBase::Reset()
{
	GameTimer = GAME_TIME;
	ElfHealth = ELF_HEALTH_MAX;
	ElfLives  = ELF_LIVES_MAX;

	for (int32 i = 0; i < NUM_MACHINES; i++)
	{
		Machines[i] = FMachine();
	}
}

void AJingleJamSandwichGameModeBase::KillElf()
{
	--ElfLives;

	if (ElfLives <= 0)
	{
		GameState = eLost;
	}
	else
	{
		// Respawn elf at starting point
		Elf->SetActorLocation(ElfStart);
		ElfHealth = ELF_HEALTH_MAX;
		// reset rotation
	}
}
