// Fill out your copyright notice in the Description page of Project Settings.

#include "Planet.h"
#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "TwinStickShooterPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "EngineUtils.h"
#include "TwinStickShooterProjectile.h"


// Sets default values
APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlanetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlanetMesh"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> tMeshObject(TEXT("/Game/Meshes/SM_Planet.SM_Planet"));

	if (tMeshObject.Succeeded())
		PlanetMesh->SetStaticMesh(tMeshObject.Object);

	PlanetMesh->SetSimulatePhysics(true);
	PlanetMesh->SetEnableGravity(false);
	PlanetMesh->GetBodyInstance()->bLockXTranslation = true;
	PlanetMesh->GetBodyInstance()->bLockYTranslation = true;
	PlanetMesh->GetBodyInstance()->bLockZTranslation = true;
	PlanetMesh->GetBodyInstance()->SetDOFLock(EDOFMode::SixDOF);
	PlanetMesh->SetConstraintMode(EDOFMode::SixDOF);
}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();

	auto tPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), int32(0));
}

// Called every frame
void APlanet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->Pull(DeltaTime);
}

void APlanet::Pull(float DeltaTime)
{
	const float Grav = -980;

	TArray<AActor*> tActorArray;

	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		auto tActor = *ActorItr;

		if (!tActor->IsValidLowLevel())
		{
			ActorArray.Remove(tActor);

			continue;
		}

		bool tValid = ActorArray.Contains<AActor*>(tActor);

		// TODO : Check by a tag set for each actor to be affected by gravity
		if (!tValid && tActor != this && (tActor->IsA(ATwinStickShooterProjectile::StaticClass()) ||
			tActor->IsA(ATwinStickShooterPawn::StaticClass()) || tActor->IsA(APlanet::StaticClass())))
		{
			tActorArray.Add(tActor);
		}
	}

	for (auto& tActor : tActorArray)
	{
		auto tMesh = Cast<UStaticMeshComponent>(tActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));

		FVector tDir = tActor->GetActorLocation() - GetActorLocation();
		FVector tDist = tActor->GetActorLocation() - GetActorLocation();
		//FVector tOrientation = tDist + GetActorLocation();
		
		tDir.Normalize();

		//FRotator tCurrRot = FRotator::MakeFromEuler(tActor->GetActorUpVector());
		//FRotator tDiffRot = FRotator::MakeFromEuler(tDir);

		//FRotator tDeltaRot = FMath::RInterpTo(tActor->GetActorRotation(), tDiffRot, DeltaTime, 0.1f);

		float tM = PlanetMesh->GetMass();
		float tm = tMesh->GetMass();

		FVector tForce = ((tM * tm * Grav) / tDist.SizeSquared()) * tDist.GetSafeNormal();
		tMesh->AddForce(tForce * 10);

		FVector under = GetActorLocation();
		FVector tUp = tDist;

		DrawDebugLine(
			GetWorld(),
			under,
			tUp,
			FColor(255, 0, 0),
			false, -1, 0,
			12.333
		);

		FVector tForward = FVector::CrossProduct(tActor->GetActorRightVector(), tUp);
		FVector tRight = FVector::CrossProduct(tUp, tForward);

		FRotator tOrient = UKismetMathLibrary::MakeRotFromZX(tUp, tActor->GetActorForwardVector());

		tActor->SetActorRotation(tOrient);
	}
}

