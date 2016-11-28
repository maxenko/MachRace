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

	lastDelta = DeltaTime;
	UpdateVelocity();
}


FVector UAutopilot::getTargetVelocity() {
	
	// get current target actor velocity velocity (zero vector if not following or no target)
	FVector currentTargetActorVelocity = Target && FollowTarget ? UX::GetRootLinearVelocity(Target) : FVector::ZeroVector;
	FVector currentTargetActorVelocityN = currentTargetActorVelocity;
	currentTargetActorVelocityN.Normalize();

	FVector targetActorLoc = Target->GetActorLocation();
	FVector ownerLoc = GetOwner()->GetActorLocation();

	// factor in target offset location (this can be a moving location in front/behind/side of target)
	TargetFollowLocation = FVector(FollowOffset, 0 , 0) + targetActorLoc; // follow location is just an X axis offset location


	//////////////////////////////////////////////////////////////////////////
	// calculate follow velocity
	//////////////////////////////////////////////////////////////////////////
	if (FollowTarget) {
		
		// if offset location doesn't match, we need to adjust velocity slightly
		// to get to that location
		if (ownerLoc.X != TargetFollowLocation.X) {

			// calc dist in X, clamp to offset to keep division no more than 1.0
			float dist = FVector::Dist(FVector(ownerLoc.X, 0, 0), FVector(TargetFollowLocation.X, 0, 0));

			// calc percentage of distance away from target follow location
			float pctOfDistToTargetFollowLoc = FMath::Clamp<float>(dist/FMath::Abs(FollowOffset), 0, 1);

			// multiplier for additional (accelerated) follow speed
			float multiplier = FollowOffsetApproachCurve ? FollowOffsetApproachCurve->GetFloatValue(pctOfDistToTargetFollowLoc) : pctOfDistToTargetFollowLoc;
			float multiplierMod = TargetFollowLocation.X < ownerLoc.X ? 1 : -1;

			float additionalSpeed = MaxFollowSpeed * multiplier * multiplierMod;

			TargetFollowVelocity = FVector(additionalSpeed, 0, 0);
		}

	} else {
		TargetFollowVelocity = FVector::ZeroVector;
	}


	//////////////////////////////////////////////////////////////////////////
	// calculate dodge velocity
	//////////////////////////////////////////////////////////////////////////
	DodgeVelocity = FVector::ZeroVector;


	//////////////////////////////////////////////////////////////////////////
	// calculate velocity to position owner in line (Y axis) with Target 
	//////////////////////////////////////////////////////////////////////////
	AlignWithTargetVelocity = FVector::ZeroVector;


	return UX::GetRootLinearVelocity(Target) + TargetFollowVelocity + DodgeVelocity + AlignWithTargetVelocity;
}


void UAutopilot::AdjustVelocityToFollowTarget(float delta) {
	if (!Target) {
		return;
	}
	
	UX::SetRootLinearVelocity(GetOwner(), getTargetVelocity());
}


void UAutopilot::Scan() {

}


void UAutopilot::UpdateVelocity() {

	TargetVelocity = getTargetVelocity();

	FVector newV = FMath::VInterpTo(UX::GetRootLinearVelocity(GetOwner()), TargetVelocity, lastDelta, VelocityChangeSpeed);

	UX::SetRootLinearVelocity(GetOwner(), newV);
}