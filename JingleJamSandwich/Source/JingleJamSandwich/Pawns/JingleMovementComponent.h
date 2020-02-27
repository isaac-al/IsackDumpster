// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JingleMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JINGLEJAMSANDWICH_API UJingleMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UJingleMovementComponent();

protected:
	void Init(AActor* OwnerActor);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FVector StartLocation;
	FVector CurrentLocation;
	FRotator StartRotation;
	FRotator CurrentRotation;

	UPROPERTY(BlueprintReadWrite)
	float Speed = 0.0f;
};
