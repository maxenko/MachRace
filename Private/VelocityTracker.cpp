// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "VelocityTracker.h"


// Sets default values for this component's properties
UVelocityTracker::UVelocityTracker() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVelocityTracker::BeginPlay() {
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UVelocityTracker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto owner = GetOwner();
	if (!owner) {
		return;
	}

	auto currentLocation = owner->GetActorLocation();

	LastVelocity = LastVelocity - currentLocation;
}