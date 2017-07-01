// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "ProximityTrigger.h"


// Sets default values for this component's properties
UProximityTrigger::UProximityTrigger()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UProximityTrigger::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UProximityTrigger::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (Target) {

		if (Target->IsValidLowLevel()) {
			auto dist = FVector::Dist(Target->GetActorLocation(), GetComponentLocation());
			if (dist < TriggerDistance) {
				InProximity.Broadcast(dist);
			}
		}

	}
}


