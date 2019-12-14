// Fill out your copyright notice in the Description page of Project Settings.


#include "Toy.h"
#include <Engine/Engine.h>
#include <ConstructorHelpers.h>
#include "JingleJamSandwichGameModeBase.h"

// Sets default values
AToy::AToy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TestMesh"));
}

// Called when the game starts or when spawned
void AToy::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString("Toy boi"));
	//TO DO : Replace this with call to RandomiseToy
	LoadMesh("Test");
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
		LoadMesh("Train");
		break;
	case 2:
		LoadMesh("Bear");
		break;
	case 3:
		LoadMesh("Ball");
		break;
	case 4:
		LoadMesh("Ball");
		break;
	case 5:
		LoadMesh("Ball");
		break;
	default:
		break;
	}

	itemType = chosenToy;
}

void AToy::LoadMesh(FString MeshName)
{
	FString Directory = "StaticMesh'/Game/Meshes/" + MeshName + "." + MeshName + "'";
	UStaticMesh* MeshAsset = LoadObject<UStaticMesh>(NULL, *Directory);
	if (MeshAsset != nullptr)
	{
		mesh->SetStaticMesh(MeshAsset);
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString("Loaded Mesh"));
	}
}