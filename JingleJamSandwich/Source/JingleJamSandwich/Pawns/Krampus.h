// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Krampus.generated.h"

class UCapsuleComponent;

UCLASS()
class JINGLEJAMSANDWICH_API AKrampus : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AKrampus();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleComp;

	UFUNCTION(BlueprintCallable)
	void LoadMesh();

	UPROPERTY(BlueprintReadWrite)
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	bool bAttack = false;

	FVector Velocity;
	float SpeedModifier = 250.0f;
	int32 MachineOverlap = 4;
	bool bElfOverlap = false;
	void MoveX(float amount);
	void MoveY(float amount);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	FVector CurrentLocation;
	FRotator CurrentRotation;
};