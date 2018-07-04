// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "X.h"
#include "AutoPilotV3.h"


// Sets default values for this component's properties
UAutoPilotV3::UAutoPilotV3() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UAutoPilotV3::BeginPlay() {
	Super::BeginPlay();
	
}

// Called every frame
void UAutoPilotV3::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	delta = DeltaTime;

	if (OwnerPhysicsComponent && Target) {
		OwnerPhysicsComponent->SetAllPhysicsLinearVelocity(getFinalVelocity());
	}
}

#pragma optimize("", off)
float UAutoPilotV3::getVelocityX(float targetSpeed, float ownerSpeed, FVector targetLoc, FVector ownerLoc) {

	float dist = UX::GetXDist(targetLoc+TargetOffset, ownerLoc);

	float thresholdMultiplier = 1.0; // by default, accelerate at full speed

	if (dist < AccelerationXThreshold) { // but if within threshold, do linear decay (we want to gradually slow down as we approach Target+TargetOffset)
		thresholdMultiplier = FMath::GetMappedRangeValueClamped(FVector2D(0, AccelerationXThreshold), FVector2D(0.0, 1.0), dist);
	}

	auto direction		= (targetLoc.X + TargetOffset.X) < ownerLoc.X ? 1 : -1; // reverse speed adjustment, depending if we're ahead or behind Target+TargetOffset
	float modifier		= (targetSpeed * MaxAccelerationXPct * thresholdMultiplier * direction); // speed up or slow down
	float easeInValue	= FMath::FInterpTo(ownerSpeed, targetSpeed + modifier, delta, VelocityAdjustmentSpeedX);

	return easeInValue;
}
#pragma optimize("", on)

float UAutoPilotV3::getVelocityY(float ownerSpeed, FVector targetLoc, FVector ownerLoc) {

	if (!AlignWithTargetInY) {
		return 0.0;
	}

	float dist = UX::GetYDist(targetLoc+TargetOffset, ownerLoc);

	float thresholdMultiplier = 1.0; // by default, accelerate at full speed

	if (dist < AccelerationYThreshold) { // but if within threshold, do linear decay (we want to gradually slow down as we approach Target+TargetOffset)
		thresholdMultiplier = FMath::GetMappedRangeValueClamped(FVector2D(0, AccelerationYThreshold), FVector2D(0.0, 1.0), dist);
	}

	float direction = (targetLoc.Y + TargetOffset.Y) < ownerLoc.Y ? -1 : 1; // reverse speed adjustment, depending if we're ahead or behind Target+TargetOffset
	float desiredSpeedY = MaxAccelerationY * thresholdMultiplier * direction;
	float easeInValue = FMath::FInterpTo(ownerSpeed, desiredSpeedY, delta, VelocityAdjustmentSpeedY);


	return easeInValue;
}

float UAutoPilotV3::getVelocityZ(float ownerSpeed, FVector targetLoc, FVector ownerLoc) {

	if (!AlignWithTargetInZ) {
		return 0.0;
	}

	float dist = UX::GetZDist(targetLoc + TargetOffset, ownerLoc);

	float thresholdMultiplier = 1.0; // by default, accelerate at full speed

	if (dist < AccelerationYThreshold) { // but if within threshold, do linear decay (we want to gradually slow down as we approach Target+TargetOffset)
		thresholdMultiplier = FMath::GetMappedRangeValueClamped(FVector2D(0, AccelerationZThreshold), FVector2D(0.0, 1.0), dist);
	}

	float direction = (targetLoc.Z + TargetOffset.Z) < ownerLoc.Z ? -1 : 1; // reverse speed adjustment, depending if we're ahead or behind Target+TargetOffset
	float desiredSpeedZ = MaxAccelerationZ * thresholdMultiplier * direction;
	float easeInValue = FMath::FInterpTo(ownerSpeed, desiredSpeedZ, delta, VelocityAdjustmentSpeedZ);


	return easeInValue;
}

FVector UAutoPilotV3::getFinalVelocity() {

	if ( !OwnerPhysicsComponent || !Target ) {
		return FVector::ZeroVector;
	}

	auto ownerVelocity	= OwnerPhysicsComponent->GetPhysicsLinearVelocity();
	auto targetVelocity = UX::GetRootLinearVelocity(Target);
	FVector targetLoc	= Target->GetActorLocation();
	FVector ownerLoc	= OwnerPhysicsComponent->GetComponentLocation();
	
	auto finalV = FVector(
		getVelocityX(targetVelocity.X, ownerVelocity.X, targetLoc, ownerLoc), 
		getVelocityY(ownerVelocity.Y, targetLoc, ownerLoc), 
		getVelocityZ(ownerVelocity.Z, targetLoc, ownerLoc)
	);

	return finalV;
}