// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode_Multiplayer.h"
#include "Pawn_Multiplayer.h"
#include "GameState_Multiplayer.h"
#include "PlayerState_Multiplayer.h"
#include "Kismet/GameplayStatics.h"
#include "TwinStickGameInstance.h"
#include "Engine\Engine.h"
#include "TwinStickShooterPawn.h"

AGameMode_Multiplayer::AGameMode_Multiplayer()
{
	DefaultPawnClass = ATwinStickShooterPawn::StaticClass();
	PlayerStateClass = APlayerState_Multiplayer::StaticClass();
	GameStateClass   = AGameState_Multiplayer::StaticClass();
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AGameMode_Multiplayer::BeginPlay()
{
	Super::BeginPlay();
}

float gm_timer = 0; 
float server_started = false;

#define STRINGIFY(argument) #argument

void AGameMode_Multiplayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ActiveControllers.Num() > 1)
	{
		ATwinStickShooterPawn* pawn_a = Cast<ATwinStickShooterPawn>(ActiveControllers[0]->GetPawn());
		ATwinStickShooterPawn* pawn_b = Cast<ATwinStickShooterPawn>(ActiveControllers[1]->GetPawn());

		QUICK_LOG_UPDATE(FString::Printf(TEXT("%s : %s"), TEXT("pawn a loc"), *pawn_a->GetActorLocation().ToString()));
		QUICK_LOG_UPDATE(FString::Printf(TEXT("%s : %s"), TEXT("pawn b loc"), *pawn_b->GetActorLocation().ToString()));
		QUICK_LOG_UPDATE(FString::Printf(TEXT("%s : %i"), TEXT("pawn a"), pawn_a->ReplicationTestCount));
		QUICK_LOG_UPDATE(FString::Printf(TEXT("%s : %i"), TEXT("pawn b"), pawn_b->ReplicationTestCount));
	}
}

void AGameMode_Multiplayer::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	ActiveControllers.Push(NewPlayer);

	ATwinStickShooterPawn* GodPawn = Cast<ATwinStickShooterPawn>(ActiveControllers[0]->GetPawn());
	
	if (GodPawn->m_eCurrentPlayerType == EPlayerType::ePlayerType_FPS)
	{
		GodPawn->OnSwitchPlayerMode();
	}
}