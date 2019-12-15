// Fill out your copyright notice in the Description page of Project Settings.


#include "JinglePlayerController.h"
#include "JingleJamSandwichGameModeBase.h"
#include <Kismet/GameplayStatics.h>
#include <WidgetBlueprintLibrary.h>
#include <UserWidget.h>
#include <JingleJamSandwich\Pawns\Krampus.h>
#include <JingleJamSandwich\Pawns\Elf.h>
#include <JingleJamSandwich\Toy\Toy.h>

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
	InputComponent->BindAction("KrampusAction", EInputEvent::IE_Released, this, &AJinglePlayerController::KrampusActionReleased);
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
	if (Elf->CurrentToy != nullptr) {
		if (Elf->MachineOverlap != EMachineColour::eColourMax)
		{
			switch (Elf->MachineOverlap)
			{
			case 0:
				gamemode->PaintToy(Elf->CurrentToy, EMachineColour::eRed);
				break;
			case 1:
				gamemode->PaintToy(Elf->CurrentToy, EMachineColour::eBlue);
				break;
			case 2:
				gamemode->PaintToy(Elf->CurrentToy, EMachineColour::eGreen);
				break;
			case 3:
				gamemode->PaintToy(Elf->CurrentToy, EMachineColour::eYellow);
				break;
			default:
				break;
			}
		}
		else if (Elf->CurrentToy->bCanBePickedUp)
		{
			Elf->CurrentToy->AttachToActor(Elf, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "L_Arm_Hand");
			Elf->CurrentToy->MovementSpeed = 0.0f;
		}
	}
}

void AJinglePlayerController::DropReleased()
{
	AJingleJamSandwichGameModeBase* gamemode = Cast<AJingleJamSandwichGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	
	if (Elf->CurrentToy != nullptr)
	{
		if (Elf->MachineOverlap == EMachineColour::eColourMax) {
			Elf->CurrentToy->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			FVector currentLocation = Elf->CurrentToy->GetActorLocation();
			Elf->CurrentToy->SetActorLocation(FVector(currentLocation.X, currentLocation.Y, 0.0f));
			Elf->CurrentToy->SetActorRotation(FRotator::ZeroRotator);
			Elf->MachineOverlap = EMachineColour::eColourMax;
			Elf->CurrentToy = nullptr;
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

void AJinglePlayerController::KrampusActionReleased()
{
	AJingleJamSandwichGameModeBase* gamemode = Cast<AJingleJamSandwichGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	
	if (Krampus->bElfOverlap)
	{
		gamemode->DamageElf();
		Krampus->bElfOverlap = false;
	}
	else if (Krampus->MachineOverlap < (int32)EMachineColour::eColourMax)
	{
		gamemode->BreakMachine((EMachineColour)Krampus->MachineOverlap);
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
