// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn_Multiplayer.h"

// Sets default values
APawn_Multiplayer::APawn_Multiplayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bReplicateMovement = true;
}

// Called when the game starts or when spawned
void APawn_Multiplayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APawn_Multiplayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APawn_Multiplayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent); 
}

// TODO:
// - Run listen server to connect 2 clients (display debug text)
// - Replicate pawn mesh