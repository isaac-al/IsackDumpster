// Fill out your copyright notice in the Description page of Project Settings.


#include "Krampus.h"
#include "Components/InputComponent.h"
#include "JingleJamSandwichGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include <Runtime\Engine\Classes\Engine\SkeletalMesh.h>
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AKrampus::AKrampus()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComp->SetCapsuleHalfHeight(100.0f);
	CapsuleComp->SetCapsuleRadius(50.0f);
	CapsuleComp->SetGenerateOverlapEvents(true);
	RootComponent = mesh;
	CapsuleComp->SetupAttachment(mesh, FName("Capsule"));
	LoadMesh();
}

// Called when the game starts or when spawned
void AKrampus::BeginPlay()
{
	Super::BeginPlay();
	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AKrampus::OnOverlapBegin);
	CapsuleComp->OnComponentEndOverlap.AddDynamic(this, &AKrampus::OnOverlapEnd);
}

// Called every frame
void AKrampus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if we have movement
	if (!Velocity.IsZero())
	{
		FVector NewLocation = GetActorLocation() + (Velocity * DeltaTime);
		FVector lookat = NewLocation;
		FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), NewLocation);
		SetActorLocation(NewLocation);
		SetActorRotation(NewRotation);
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
}

void AKrampus::MoveX(float amount)
{
	Velocity.X = FMath::Clamp(amount, -1.0f, 1.0f) * SpeedModifier;
}

void AKrampus::MoveY(float amount)
{
	Velocity.Y = FMath::Clamp(amount, -1.0f, 1.0f) * SpeedModifier;
}

void AKrampus::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AJingleJamSandwichGameModeBase* gamemode = Cast<AJingleJamSandwichGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	FString triggerName = FString(*OtherActor->GetName());

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString("OVERLAP WITH ACTOR: " + triggerName));

	if (triggerName.Contains("GREEN"))
	{
		MachineOverlap = (int32)EMachineColour::eGreen;
	}
	else if (triggerName.Contains("RED"))
	{
		MachineOverlap = (int32)EMachineColour::eRed;
	}
	else if (triggerName.Contains("BLUE"))
	{
		MachineOverlap = (int32)EMachineColour::eBlue;
	}
	else if (triggerName.Contains("YELLOW"))
	{
		MachineOverlap = (int32)EMachineColour::eYellow;
	}
	else if (triggerName.Contains("ELF"))
	{
		bElfOverlap = true;
	}
}

void AKrampus::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bElfOverlap = false;
	MachineOverlap = (int32)EMachineColour::eColourMax;
}