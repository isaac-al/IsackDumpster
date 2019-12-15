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
#include <DefaultPawnOverride.h>
#include <Materials/MaterialInstance.h>
#include <Particles/Emitter.h>
#include <Particles/ParticleEmitter.h>
#include <Particles/ParticleSystemComponent.h>
#include <Components/AudioComponent.h>

AJingleJamSandwichGameModeBase::AJingleJamSandwichGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	DefaultPawnClass = ADefaultPawnOverride::StaticClass();
	PlayerControllerClass = AJinglePlayerController::StaticClass();
	LoadAudio();
}

void AJingleJamSandwichGameModeBase::StartGame()
{
	// iterate through players
	TArray<AActor*> controllers;

	TSubclassOf<AJinglePlayerController> jingleControllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AJinglePlayerController::StaticClass(), controllers);

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

	bPleaseOpenMainThanks = false;
	bPleaseOpenGameOverThanks = false;
	// TODO: Display HUD
	Reset();
	MakeList();
	// check it twice
}

void AJingleJamSandwichGameModeBase::LoadAudio()
{
	FixingCue = LoadObject<USoundCue>(NULL, *FString("SoundCue'/Game/Sounds/FixingCue.FixingCue'"));
	FixCompleteCue = LoadObject<USoundCue>(NULL, *FString("SoundCue'/Game/Sounds/FixCompletedCue.FixCompletedCue'"));
	SabotageCue = LoadObject<USoundCue>(NULL, *FString("SoundCue'/Game/Sounds/SabotageCue.SabotageCue'"));
	DeliveredCue = LoadObject<USoundCue>(NULL, *FString("SoundCue'/Game/Sounds/DeliverCue.DeliverCue'"));
	ItemPickUpCue = LoadObject<USoundCue>(NULL, *FString("SoundCue'/Game/Sounds/Item_Pickup_Cue.Item_Pickup_Cue'"));
	KrampusAttackQue = LoadObject<USoundCue>(NULL, *FString("SoundCue'/Game/Sounds/Krampus_Attack_Cue.Krampus_Attack_Cue'"));
	LifeLostQue = LoadObject<USoundCue>(NULL, *FString("SoundCue'/Game/Sounds/Life_Lost_Cue.Life_Lost_Cue'"));
	PaintItemQue = LoadObject<USoundCue>(NULL, *FString("SoundCue'/Game/Sounds/Paint_Item_Cue.Paint_Item_Cue'"));
	PauseQue = LoadObject<USoundCue>(NULL, *FString("SoundCue'/Game/Sounds/Pause_Cue.Pause_Cue'"));
	ScribbleOutCue = LoadObject<USoundCue>(NULL, *FString("SoundCue'/Game/Sounds/Scribble_Out_Item_Cue.Scribble_Out_Item_Cue'"));

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));                        
	AudioComponent->SetupAttachment(RootComponent);
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
	AudioComponent->SetSound(PauseQue);
	AudioComponent->Play();
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
	GetWorld()->SpawnActor<AToy>(AToy::StaticClass(), FVector(46.468418f, 899.603516f, 212.685196f), FRotator::ZeroRotator);
}

void AJingleJamSandwichGameModeBase::DamageElf()
{
	if (Krampus->bElfOverlap)
	{
		--ElfHealth;

		if (ElfHealth <= 0)
		{
			KillElf();
		}

		Krampus->bElfOverlap = false;
	}	
}

void AJingleJamSandwichGameModeBase::MakeList()
{
	ItemList.Empty();

	for (int32 i = 0; i < 4; ++i)
	{
		FToyItem item;
		item.ItemType = FMath::RandRange(1, 8);
		item.colour = (EMachineColour)FMath::RandRange(0, 3);
		ItemList.Add(item);
	}
}

void AJingleJamSandwichGameModeBase::PaintToy(AToy* InToy, EMachineColour InColour)
{
	if (Machines[(int32)InColour].Broken == false) {
		// TODO: set mesh colour
		if (InToy && InToy->IsValidLowLevel())
		{
			AudioComponent->SetSound(PaintItemQue);
			InToy->colour = InColour;
			switch (InColour)
			{
			case eRed:
				ChangeMaterial("Red", InToy);
				AudioComponent->Play();
				break;
			case eBlue:
				ChangeMaterial("Blue", InToy);
				AudioComponent->Play();
				break;
			case eGreen:
				ChangeMaterial("Green", InToy);
				AudioComponent->Play();
				break;
			case eYellow:
				ChangeMaterial("Yellow", InToy);
				AudioComponent->Play();
				break;
			default:
				break;
			}
		}
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
			AudioComponent->SetSound(DeliveredCue);
			AudioComponent->Play();
			ItemList.RemoveAt(i);
			validToy = true;
			Score += DELIVER_POINTS;
			Elf->CurrentToy->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			FVector currentLocation = Elf->CurrentToy->GetActorLocation();
			Elf->CurrentToy->SetActorLocation(FVector(Elf->GetActorLocation().X, -3090.0f, 140.0f));
			Elf->CurrentToy->MovementSpeed = 100.0f;
			Elf->CurrentToy->bCanBePickedUp = false;
			Elf->CurrentToy = nullptr;
			AudioComponent->SetSound(ScribbleOutCue);
			AudioComponent->Play();
			break;
		}
	}

	if (!validToy)
		return;

	if (ItemList.Num() <= 0)
	{
		Score += LIST_CLEAR_POINTS;
		MakeList();
	}
	
}

void AJingleJamSandwichGameModeBase::DestroyToy(AToy* InToy)
{
	if (InToy && InToy->IsValidLowLevel())
	{
		InToy->BeginDestroy();
	}
}

void AJingleJamSandwichGameModeBase::ChangeMaterial(FString MaterialName, AToy* InToy)
{
	UMaterialInstance* MaterialAsset = LoadObject<UMaterialInstance>(NULL, *FString("MaterialInstanceConstant'/Game/Materials/" + MaterialName + "." + MaterialName + "'"));
	if (MaterialAsset != nullptr)
	{
		InToy->mesh->SetMaterial(0, MaterialAsset);
	}
}

void AJingleJamSandwichGameModeBase::BreakMachine(EMachineColour InMachineColour)
{
	Machines[(int32)InMachineColour].Broken = true;
	TArray<AActor*> Emitters;
	TSubclassOf<AEmitter> Emitter;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEmitter::StaticClass(), Emitters);
	FString CurrentMachineColour;
	switch (InMachineColour)
	{
	case eRed:
		CurrentMachineColour = "Red";
		break;
	case eBlue:
		CurrentMachineColour = "Blue";
		break;
	case eGreen:
		CurrentMachineColour = "Green";
		break;
	case eYellow:
		CurrentMachineColour = "Yellow";
		break;
	default:
		CurrentMachineColour = "";
		break;
	}
	for(int i = 0; i < Emitters.Num(); i++){
		FString EmitterName = Emitters[i]->GetFullName();
		if (EmitterName.Contains(CurrentMachineColour)) {
			AudioComponent->SetSound(SabotageCue);
			AudioComponent->Play();
			UParticleSystemComponent* particle = Cast<UParticleSystemComponent>(Emitters[i]->GetComponentByClass(UParticleSystemComponent::StaticClass()));
			particle->ActivateSystem();
		}
	}
}

void AJingleJamSandwichGameModeBase::RepairMachine(EMachineColour InMachineColour)
{
	if (Machines[(int32)InMachineColour].Broken == true) {
		Machines[(int32)InMachineColour].RepairTime += DeltaTime;
		AudioComponent->SetSound(FixingCue);
		AudioComponent->Play();
		RepairTime = Machines[(int32)InMachineColour].RepairTime;

		if (Machines[(int32)InMachineColour].RepairTime >= MAX_REPAIR_TIME)
		{
			Machines[(int32)InMachineColour].RepairTime = 0;
			Machines[(int32)InMachineColour].Broken = false;
			TArray<AActor*> Emitters;
			TSubclassOf<AEmitter> Emitter;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEmitter::StaticClass(), Emitters);
			FString CurrentMachineColour;
			switch (InMachineColour)
			{
			case eRed:
				CurrentMachineColour = "Red";
				break;
			case eBlue:
				CurrentMachineColour = "Blue";
				break;
			case eGreen:
				CurrentMachineColour = "Green";
				break;
			case eYellow:
				CurrentMachineColour = "Yellow";
				break;
			default:
				CurrentMachineColour = "";
				break;
			}
			for (int i = 0; i < Emitters.Num(); i++) {
				FString EmitterName = Emitters[i]->GetFullName();
				if (EmitterName.Contains(CurrentMachineColour)) {
					UParticleSystemComponent* particle = Cast<UParticleSystemComponent>(Emitters[i]->GetComponentByClass(UParticleSystemComponent::StaticClass()));
					AudioComponent->Stop();
					AudioComponent->SetSound(FixCompleteCue);
					AudioComponent->Play();
					particle->DeactivateSystem();
				}
			}
		}
	}
}

void AJingleJamSandwichGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	bPleaseOpenMainThanks = true;
	bPleaseOpenPauseThanks = false;
	bPleaseOpenGameOverThanks = false;


	TArray<AActor*> Krampai;
	TSubclassOf<AKrampus> krampus;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AKrampus::StaticClass(), Krampai);
	Krampus = Cast<AKrampus>(Krampai[0]);

	TArray<AActor*> Elves;
	TSubclassOf<AElf> elf;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AElf::StaticClass(), Elves);
	Elf = Cast<AElf>(Elves[0]);

	TArray<AActor*> Emitters;
	TSubclassOf<AEmitter> emitter;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEmitter::StaticClass(), Emitters);
	for (int i = 0; i < Emitters.Num(); i++) {
		FString EmitterName = Emitters[i]->GetFullName();
		if (EmitterName.Contains("Fire") == false) {
			UParticleSystemComponent* particle = Cast<UParticleSystemComponent>(Emitters[i]->GetComponentByClass(UParticleSystemComponent::StaticClass()));
			particle->DeactivateSystem();
		}
	}

	Reset();
}

void AJingleJamSandwichGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DeltaTime = DeltaSeconds;

	if (Elf->bRepairingMachine) {
		RepairMachine((EMachineColour)Elf->MachineOverlap);
	}

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
	ElfHealthNormal = ((float)ElfHealth / (float)ELF_HEALTH_MAX);
	bShowHUD = true;
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString("Time left: " + FString::SanitizeFloat(GameTimer)));
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString("Health left: " + FString::FromInt(ElfHealth)));

	if (GameTimer <= 0.0f)
	{
		// Game over biiiitch
		GameState = eWon;
		bPleaseOpenGameOverThanks = true;
	}

	int32 time = FMath::FloorToInt(GameTimer);

	ToyCooldown -= DeltaTime;

	if (ToyCooldown <= 0.0f)
	{
		SpawnToy();
		ToyCooldown = FMath::RandRange(1, 10);
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
	ElfHealth = ELF_HEALTH_MAX;

	if (ElfLives <= 0)
	{
		GameState = eLost;
	}
	else
	{
		// Respawn elf at starting point
		if (Elf)
		{
			Elf->SetActorLocation(Elf->StartLocation);
			Elf->SetActorRotation(Elf->StartRotation);
			Score = 0;
		}
		// reset rotation
	}
}
