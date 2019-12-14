// Fill out your copyright notice in the Description page of Project Settings.


#include "Krampus.h"
#include "Components/InputComponent.h"

// Sets default values
AKrampus::AKrampus()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = mesh;
	LoadMesh();
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

	//if we have movement
	if (!Velocity.IsZero())
	{
		FVector NewLocation = GetActorLocation() + (Velocity * DeltaTime);
		SetActorLocation(NewLocation);
	}
}

// Called to bind functionality to input
void AKrampus::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("Krampus_X", this, &AKrampus::MoveX);
	InputComponent->BindAxis("Krampus_Y", this, &AKrampus::MoveY);
}

void AKrampus::LoadMesh()
{
	FString Directory = "StaticMesh'/Game/Meshes/Krampus.Krampus'";
	UStaticMesh* MeshAsset = LoadObject<UStaticMesh>(NULL, *Directory);
	if (MeshAsset != nullptr)
	{
		mesh->SetStaticMesh(MeshAsset);
	}
}

void AKrampus::MoveX(float amount)
{
	Velocity.X = FMath::Clamp(amount, -1.0f, 1.0f) * SpeedModifier;
}

void AKrampus::MoveY(float amount)
{
	Velocity.Y = FMath::Clamp(amount, -1.0f, 1.0f) * SpeedModifier;
}