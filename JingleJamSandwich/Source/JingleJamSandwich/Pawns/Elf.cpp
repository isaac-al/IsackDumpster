// Fill out your copyright notice in the Description page of Project Settings.


#include "Elf.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Toy/Toy.h"
#include "JingleJamSandwichGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AElf::AElf()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComp->SetCapsuleHalfHeight(500.0f);
	CapsuleComp->SetCapsuleRadius(200.0f);
	CapsuleComp->SetGenerateOverlapEvents(true);
	RootComponent = mesh;
	CapsuleComp->SetupAttachment(mesh, FName("Capsule"));
	
	LoadMesh();
}

// Called when the game starts or when spawned
void AElf::BeginPlay()
{
	Super::BeginPlay();
	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AElf::OnOverlapBegin);
}

// Called every frame
void AElf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString("ELF Velocity"));
	//if we have movement
	if (!Velocity.IsZero())
	{
		FVector NewLocation = GetActorLocation() + (Velocity * DeltaTime);
		SetActorLocation(NewLocation);
	}
}

void AElf::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("Elf_X", this, &AElf::MoveX);
	InputComponent->BindAxis("Elf_Y", this, &AElf::MoveY);

}

void AElf::LoadMesh()
{
	USkeletalMesh* MeshAsset = LoadObject<USkeletalMesh>(NULL, *FString("SkeletalMesh'/Game/Characters/Elf/Elf_01_SK.Elf_01_SK'"));
	if (MeshAsset != nullptr)
	{
		mesh->SetSkeletalMesh(MeshAsset);
	}
}

void AElf::MoveX(float amount)
{
	Velocity.X = FMath::Clamp(amount, -1.0f, 1.0f) * SpeedModifier;
}

void AElf::MoveY(float amount)
{
	Velocity.Y = FMath::Clamp(amount, -1.0f, 1.0f) * SpeedModifier;
}

void AElf::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AJingleJamSandwichGameModeBase* gamemode = Cast<AJingleJamSandwichGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	FString triggerName = FString(*OtherActor->GetName());

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString("OVERLAP WITH ACTOR: " + triggerName));

	FString LHS = "";
	FString RHS = "";

	triggerName.Split("_", &LHS, &RHS);

	if (RHS.Contains("GREEN"))
	{
		gamemode->PaintToy(CurrentToy, EMachineColour::eGreen);
	}
	else if (RHS.Contains("RED"))
	{
		gamemode->PaintToy(CurrentToy, EMachineColour::eRed);
	}
	else if (RHS.Contains("BLUE"))
	{
		gamemode->PaintToy(CurrentToy, EMachineColour::eBlue);
	}
	else if (RHS.Contains("YELLOW"))
	{
		gamemode->PaintToy(CurrentToy, EMachineColour::eYellow);
	}
	else if (RHS.Contains("DELIVER"))
	{
		gamemode->DeliverToy(CurrentToy);
	}
	else if (RHS.Contains("TRASH"))
	{
		gamemode->DestroyToy(CurrentToy);
	}
}