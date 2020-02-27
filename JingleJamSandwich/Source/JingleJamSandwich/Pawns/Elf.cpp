// Fill out your copyright notice in the Description page of Project Settings.


#include "Elf.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Toy/Toy.h"
#include "JingleJamSandwichGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "JingleMovementComponent.h"

// Sets default values
AElf::AElf()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	//mesh->SetSimulatePhysics(true);
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComp->SetCapsuleHalfHeight(30.0f);
	CapsuleComp->SetCapsuleRadius(60.0f);
	//RootComponent = mesh;
	mesh->SetupAttachment(CapsuleComp, FName("MeshFella"));
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComp->SetGenerateOverlapEvents(true);

	CapsuleComp->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	CapsuleComp->CanCharacterStepUpOn = ECB_No;
	CapsuleComp->SetSimulatePhysics(true);
	CapsuleComp->SetEnableGravity(false);
	RootComponent = CapsuleComp;

	LoadMesh();
}

// Called when the game starts or when spawned
void AElf::BeginPlay()
{
	Super::BeginPlay();
	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AElf::OnOverlapBegin);
	CapsuleComp->OnComponentEndOverlap.AddDynamic(this, &AElf::OnOverlapEnd);

	MovementComponent->Init(this);
}

// Called every frame
void AElf::Tick(float DeltaTime)
{
	CapsuleComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
	CapsuleComp->SetPhysicsAngularVelocity(FVector::ZeroVector);
	Super::Tick(DeltaTime);
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString("ELF Velocity"));
	//if we have movement
	if (!Velocity.IsZero())
	{
		CurrentLocation = GetActorLocation() + (Velocity * DeltaTime);
		FVector lookat = CurrentLocation;
		CurrentRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CurrentLocation);
	}

	SetActorLocation(CurrentLocation);

	FRotator budge = FRotator::MakeFromEuler(FVector(0.0f, 0.0f, -90.0f));

	mesh->SetRelativeRotation(CurrentRotation + budge);
	Speed = Velocity.GetSafeNormal().Size() * 100.0f;

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


	AToy* OverlapToy = Cast<AToy>(OtherActor);

	if (!OverlappedToys.Contains(OverlapToy) && OverlapToy != CurrentToy)
	{
		OverlappedToys.Add(OverlapToy);
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
}

void AElf::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AJingleJamSandwichGameModeBase* gamemode = Cast<AJingleJamSandwichGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	AToy* OverlapToy = Cast<AToy>(OtherActor);

	if (OverlappedToys.Contains(OverlapToy))
	{
		OverlappedToys.Remove(OverlapToy);
	}

	gamemode->RepairTime = 0.0f;

	for (int32 i = 0; i < 4; i++)
	{
		gamemode->Machines[i].RepairTime = 0.0f;
	}

	MachineOverlap = (int32)EMachineColour::eColourMax;
	bDeliveryOverlap = false;
}

void AElf::Restart()
{
	SetActorLocation(StartLocation);
	SetActorRotation(StartRotation);
}