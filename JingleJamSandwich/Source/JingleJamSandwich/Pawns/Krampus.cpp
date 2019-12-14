// Fill out your copyright notice in the Description page of Project Settings.


#include "Krampus.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include <Runtime\Engine\Classes\Engine\SkeletalMesh.h>

// Sets default values
AKrampus::AKrampus()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SetRootComponent(mesh);
	//CapsuleComp->AttachToComponent(RootComponent);
	CapsuleComp->SetCapsuleHalfHeight(500.0f);
	CapsuleComp->SetCapsuleRadius(200.0f);
	CapsuleComp->SetGenerateOverlapEvents(true);
	//mesh->SetupAttachment(RootComponent);
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
	//load Skeletal mesh
	USkeletalMesh* MeshAsset = LoadObject<USkeletalMesh>(NULL, *FString("SkeletalMesh'/Game/Characters/Krampus/Krampus_01_SK.Krampus_01_SK'"));
	if (MeshAsset != nullptr)
	{
		mesh->SetSkeletalMesh(MeshAsset);
	}
	//Load skeleton
	//USkeleton* Skeleton = LoadObject<USkeleton>(NULL, *FString("Skeleton'/Game/Characters/Krampus/Krampus_01_SK_Skeleton.Krampus_01_SK_Skeleton'"));
	//if (Skeleton != nullptr)
	//{
	//	skeleton = Skeleton;
	//}
	////load physics asset
	//UPhysicsAsset* PhysicsAsset = LoadObject<UPhysicsAsset>(NULL, *FString("PhysicsAsset'/Game/Characters/Krampus/Krampus_01_SK_PhysicsAsset.Krampus_01_SK_PhysicsAsset'"));
	//if (PhysicsAsset != nullptr)
	//{
	//	physicsAsset = PhysicsAsset;
	//}
}

void AKrampus::MoveX(float amount)
{
	Velocity.X = FMath::Clamp(amount, -1.0f, 1.0f) * SpeedModifier;
}

void AKrampus::MoveY(float amount)
{
	Velocity.Y = FMath::Clamp(amount, -1.0f, 1.0f) * SpeedModifier;
}