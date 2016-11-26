// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "Autopilot.h"
#include "X.h"


// Sets default values for this component's properties
UAutopilot::UAutopilot() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAutopilot::BeginPlay() {
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAutopilot::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	
}


void UAutopilot::AdjustVelocityToFollowTarget(float delta) {
	if (!Target) {
		return;
	}

	// target location (Target Loc + FollowDistance)
	FVector targetLoc = Target->GetActorLocation() + FVector(FllowDistance, 0, 0);

	// distance between current location and target location in X axis only
	float dist = FVector::Dist( 
		FVector(Target->GetActorLocation().X,0,0), 
		FVector(targetLoc.X, 0, 0 )
		);



	FVector currentVelocity = UX::GetRootLinearVelocity(Target);

	UX::SetRootLinearVelocity(
		Target,
		FMath::VInterpTo(currentVelocity, Velocity, delta, NavigationSpeed)
	);
}


void UAutopilot::keepUpWithTarget() {
	float targetFollowDist = Target->GetActorLocation().X + FllowDistance;
	if (GetOwner()->GetActorLocation().X != targetFollowDist) {

	}
}