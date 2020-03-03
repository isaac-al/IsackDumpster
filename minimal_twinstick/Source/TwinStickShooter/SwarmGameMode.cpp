// Fill out your copyright notice in the Description page of Project Settings.

#include "SwarmGameMode.h"
#include "SwarmGameWave.h"
#include "TwinStickShooterPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "SwarmGameState.h"
#include "TimerManager.h"
#include "Runtime/Engine/Public/EngineGlobals.h"
#include "FlockEmitter.h"


ASwarmGameMode::ASwarmGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our character class
	DefaultPawnClass = ASwarmGameMode::StaticClass();
	GameStateClass = ASwarmGameState::StaticClass();
}

void ASwarmGameMode::BeginPlay()
{
	Super::BeginPlay();
	// TODO : Initiate waves, begin wave one
	UE_LOG(LogTemp, Warning, TEXT("ASwarmGameMode::BeginPlay()"));

	//AFlockEmitter* const flock_default = GetWorld()->SpawnActor<AFlockEmitter>(FActorSpawnParameters());
	//FBoidState default_state;
	//default_state.Weights = FWeights(1.0f, .8f, .6f, 0.7f, 0.3f, 1.0f, 0.0f);
	//default_state.RadiusFromTarget = 0.0f/*500.0f*/;
	//default_state.bIsPlayer = false;
	//flock_default->Init(EEnemyBehavaiour::eNavigate | EEnemyBehavaiour::eShoot, default_state, 8, FLinearColor::Blue, 1.0f);

	/*AFlockEmitter* const flock_shooter = GetWorld()->SpawnActor<AFlockEmitter>(FActorSpawnParameters());
	FBoidState shooter_state;
	shooter_state.Weights = FWeights(.5f, 1.0f, 0.2f, 0.4f, 0.3f, 1.0f, 0.0f);
	shooter_state.RadiusFromTarget = 300.0f;
	flock_shooter->Init(EEnemyBehavaiour::eNavigate | EEnemyBehavaiour::eShoot, shooter_state, 20, FLinearColor::Red, 1.8f);*/

	//ATwinStickShooterPawn* pawn = NewObject<ATwinStickShooterPawn>(GetWorld(), TEXT("PlayerPawn"));

	FActorSpawnParameters params;
	params.Name = TEXT("PlayerPawn");
	FTransform transform;
	

}

// Called every frame
void ASwarmGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASwarmGameMode::GenericPlayerInitialization(AController* C)
{
	Super::GenericPlayerInitialization(C);

	FActorSpawnParameters params;
	params.Name = FName(TEXT("PlayerPawn"));
	params.bHideFromSceneOutliner = false;
	FTransform initalSpawn;
	initalSpawn.SetLocation(FVector::UpVector * 150.0f);
	ATwinStickShooterPawn* pawn = GetWorld()->SpawnActorDeferred<ATwinStickShooterPawn>(ATwinStickShooterPawn::StaticClass(), initalSpawn, GetOwner(), (APawn*) nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	pawn->FinishSpawning(initalSpawn);
	C->Possess(pawn);
}

// TODO: 
//		- Implement first person camera pawn to walk about
//		- Make bees swarm around hive
//      - If player moves close to hive, bees attack!
//      - Distract bees with fake danger, to get honey!