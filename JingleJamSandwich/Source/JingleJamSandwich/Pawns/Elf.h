// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Elf.generated.h"

class UCapsuleComponent;
class AToy;
class AJingleJamSandwichGameModeBase;
enum EMachineColour;

UCLASS()
class JINGLEJAMSANDWICH_API AElf : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AElf();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadWrite)
	USkeletalMeshComponent* mesh;

	UPROPERTY(BlueprintReadWrite)
	UCapsuleComponent* CapsuleComp;

	bool bRepairingMachine = false;

	UPROPERTY()
		AToy* CurrentToy = nullptr;

	UFUNCTION(BlueprintCallable)
	void LoadMesh();

	FVector Velocity = FVector::ZeroVector;
	float SpeedModifier = 250.0f;
	int32 MachineOverlap = 4;
	bool bDeliveryOverlap = false;
	void MoveX(float amount);
	void MoveY(float amount);

	void PickUpToy(AToy* OverlapToy);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:


};
