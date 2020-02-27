// Fill out your copyright notice in the Description page of Project Settings.


#include "Toy.h"
#include <Engine/Engine.h>
#include <ConstructorHelpers.h>
#include "JingleJamSandwichGameModeBase.h"
#include <Components/BoxComponent.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
AToy::AToy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TestMesh"));
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxComponent->SetGenerateOverlapEvents(true);

	BoxComponent->SetupAttachment(mesh, FName("BoxCollider"));
	BoxComponent->SetWorldScale3D(FVector::OneVector * 2.5f);
	BoxComponent->SetVisibility(true);
}

// Called when the game starts or when spawned
void AToy::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AToy::OnOverlapBegin);
	//TO DO : Replace this with call to RandomiseToy
	RandomiseToy();
	bCanBePickedUp = true;
}

// Called every frame
void AToy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector NewLocation = GetActorLocation() + (FVector(-MovementSpeed, 0, 0) * DeltaTime);
	SetActorLocation(NewLocation);
}

void AToy::RandomiseToy()
{
	int chosenToy = FMath::RandRange(1, 7);

	switch (chosenToy) {
	case 1:
		LoadMesh("Car");
		mesh->SetWorldScale3D(FVector(1.5f, 1.5f, 1.5f));
		break;
	case 2:
		LoadMesh("Bear");
		mesh->SetWorldScale3D(FVector(1.5f, 1.5f, 1.5f));
		break;
	case 3:
		LoadMesh("Ball");
		break;
	case 4:
		LoadMesh("Candy_Cane");
		break;
	case 5:
		LoadMesh("Game");
		mesh->SetWorldScale3D(FVector(2.5f, 2.5f, 2.5f));
		break;
	case 6:
		LoadMesh("Book");
		break;
	case 7:
		LoadMesh("Robot");
		mesh->SetWorldScale3D(FVector(1.5f, 1.5f, 1.5f));
		break;
	default:
		break;
	}

	itemType = chosenToy;
}

void AToy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString triggerName = FString(*OtherActor->GetName());
	AJingleJamSandwichGameModeBase* gamemode = Cast<AJingleJamSandwichGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (triggerName.Contains("TRASH"))
	{
		gamemode->DestroyToy(this);
	}
}

void AToy::LoadMesh(FString MeshName)
{
	FString Directory = "StaticMesh'/Game/Meshes/Props/" + MeshName + "." + MeshName + "'";
	UStaticMesh* MeshAsset = LoadObject<UStaticMesh>(NULL, *Directory);
	if (MeshAsset != nullptr)
	{
		mesh->SetStaticMesh(MeshAsset);
	}
}