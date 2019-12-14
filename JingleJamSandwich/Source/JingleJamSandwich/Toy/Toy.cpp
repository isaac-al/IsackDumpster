// Fill out your copyright notice in the Description page of Project Settings.


#include "Toy.h"
#include <Engine/Engine.h>

// Sets default values
AToy::AToy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AToy::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString("Toy boi"));
}

// Called every frame
void AToy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AToy::RandomiseToy()
{
	int chosenToy = FMath::RandRange(0, 5);

	switch (chosenToy) {
	case 1:
		
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	default:
		break;
	}
}




