// Fill out your copyright notice in the Description page of Project Settings.


#include "JingleMovementComponent.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UJingleMovementComponent::UJingleMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UJingleMovementComponent::Init(AActor* OwnerActor)
{
	CurrentLocation = StartLocation = OwnerActor->GetActorLocation();
	CurrentRotation = StartRotation = OwnerActor->GetActorRotation();
}

// Called every frame
void UJingleMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

