// Fill out your copyright notice in the Description page of Project Settings.


#include "Elf.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Toy/Toy.h"
#include "JingleJamSandwichGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AElf::AElf()
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
		FVector lookat = NewLocation;
		FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), NewLocation);
		SetActorLocation(NewLocation);
		SetActorRotation(NewRotation);
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

void AElf::PickUpToy(AToy* OverlapToy)
{
	if (CurrentToy == nullptr) 
	{
		CurrentToy = OverlapToy;
	}
}

void AElf::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AJingleJamSandwichGameModeBase* gamemode = Cast<AJingleJamSandwichGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	FString triggerName = FString(*OtherActor->GetName());

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString("OVERLAP WITH ACTOR: " + triggerName));

	AToy* OverlapToy = Cast<AToy>(OtherActor);

	if (OverlapToy != nullptr) {
		PickUpToy(OverlapToy);
	}

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
	else if (triggerName.Contains("DELIVER"))
	{
		bDeliveryOverlap = true;
		gamemode->DeliverToy(CurrentToy);
	}
	else if (triggerName.Contains("TRASH"))
	{
		gamemode->DestroyToy(CurrentToy);
	}
	else if (triggerName.Contains("krampus"))
	{
		gamemode->DamageElf();
	}
}

void AElf::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	MachineOverlap = (int32)EMachineColour::eColourMax;
	bDeliveryOverlap = false;
	CurrentToy = nullptr;
}
