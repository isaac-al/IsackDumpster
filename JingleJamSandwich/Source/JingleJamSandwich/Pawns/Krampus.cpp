// Fill out your copyright notice in the Description page of Project Settings.


#include "Krampus.h"

// Sets default values
AKrampus::AKrampus()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AKrampus::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKrampus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AKrampus::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

