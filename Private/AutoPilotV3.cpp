// Copyright 2015 - Max Enko

#include "AutoPilotV3.h"
#include "MachRace.h"
#include "X.h"
#include "DrawDebugHelpers.h"



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

		if (ScanAutomatically) {
			AutoScan();
		}

		OwnerPhysicsComponent->SetAllPhysicsLinearVelocity(getFinalVelocity());
	}
}

#pragma region Velocity Calculations

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

float UAutoPilotV3::getFollowTargetVelocityY(float ownerSpeed, FVector targetLoc, FVector ownerLoc) {

	if (!AlignWithTargetInY) {
		return 0.0;
	}

	auto offsetTarget = targetLoc + TargetOffset;

	float dist = UX::GetYDist(offsetTarget, ownerLoc);

	float thresholdMultiplier = 1.0; // by default, accelerate at full speed

	if (dist < AccelerationYThreshold) { // but if within threshold, do linear decay (we want to gradually slow down as we approach Target+TargetOffset)
		thresholdMultiplier = FMath::GetMappedRangeValueClamped(FVector2D(0, AccelerationYThreshold), FVector2D(0.0, 1.0), dist);
	}

	float direction = offsetTarget.Y < ownerLoc.Y ? -1 : 1; // reverse speed adjustment, depending if we're ahead or behind Target+TargetOffset
	float desiredSpeedY = MaxAccelerationY * thresholdMultiplier * direction;
	float easeInValue = FMath::FInterpTo(ownerSpeed, desiredSpeedY, delta, VelocityAdjustmentSpeedY);

	return easeInValue;
}

#pragma optimize("", on)

float UAutoPilotV3::getDodgeVelocityY(float ownerSpeed, FVector ownerLoc) {

	float dist = UX::GetYDist(LastClearPathScanLoc, ownerLoc);

	float thresholdMultiplier = 1.0; // by default, accelerate at full speed

	if (dist < AccelerationYThreshold) { // but if within threshold, do linear decay (we want to gradually slow down as we approach Target+TargetOffset)
		thresholdMultiplier = FMath::GetMappedRangeValueClamped(FVector2D(0, AccelerationYThreshold), FVector2D(0.0, 1.0), dist);
	}

	float direction = (LastClearPathScanLoc.Y) < ownerLoc.Y ? -1 : 1; // reverse speed adjustment, depending if we're ahead or behind Target+TargetOffset
	float desiredSpeedY = MaxAccelerationY * thresholdMultiplier * direction;
	float easeInValue = FMath::FInterpTo(ownerSpeed, desiredSpeedY, delta, VelocityAdjustmentSpeedY);

	return easeInValue;
}

float UAutoPilotV3::getVelocityZ(float ownerSpeed, FVector targetLoc, FVector ownerLoc) {

	if (!AlignWithTargetInZ) {
		return 0.0;
	}
	auto offsetTarget = targetLoc + TargetOffset;

	float dist = UX::GetZDist(offsetTarget, ownerLoc);

	float thresholdMultiplier = 1.0; // by default, accelerate at full speed

	if (dist < AccelerationYThreshold) { // but if within threshold, do linear decay (we want to gradually slow down as we approach Target+TargetOffset)
		thresholdMultiplier = FMath::GetMappedRangeValueClamped(FVector2D(0, AccelerationZThreshold), FVector2D(0.0, 1.0), dist);
	}

	float direction = (offsetTarget.Z) < ownerLoc.Z ? -1 : 1; // reverse speed adjustment, depending if we're above or below Target+TargetOffset
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

	float velocityY = 0.0;

	if (Intent == AutopilotIntent::ResumeNonDodgeBehavior) {
		velocityY = getFollowTargetVelocityY(ownerVelocity.Y, targetLoc, ownerLoc);
	} else {
		velocityY = getDodgeVelocityY(ownerVelocity.Y, ownerLoc);
	}
	
	auto finalV = FVector(
		getVelocityX(targetVelocity.X, ownerVelocity.X, targetLoc, ownerLoc), 
		velocityY,
		getVelocityZ(ownerVelocity.Z, targetLoc, ownerLoc)
	);

	return finalV;
}
#pragma endregion

void UAutoPilotV3::Scan(FVector from, FVector to, float scanRadius, TArray<AActor*> ignoredActors) {

	auto singleSphereScanResults = filterHits(sphereTrace(from, to, scanRadius, ignoredActors));

	bool doDoubleSphereTraceThisTick = false;

	if (scheduleDoubleSphereTraceNextTick) { // was double sphere trace scheduled during previous tick?
		doDoubleSphereTraceThisTick = true;
		scheduleDoubleSphereTraceNextTick = false;
	}

	if (singleSphereScanResults.Num() <= 0) {

		scanWidth = 1; // reset scanWidth to default.

		// broadcast change if not already clear
		if (PathCondition != AutopilotPathCondition::Clear) {
			PathCondition = AutopilotPathCondition::Clear;
			OnPathConditionChange.Broadcast(PathCondition);
		}

		if (Intent != AutopilotIntent::ResumeNonDodgeBehavior) {
			Intent = AutopilotIntent::ResumeNonDodgeBehavior;
		}
		
	} else {

		// broadcast change if not already blocked
		if (PathCondition != AutopilotPathCondition::Blocked) {
			PathCondition = AutopilotPathCondition::Blocked;
			OnPathConditionChange.Broadcast(PathCondition);
		}

		if (Intent != AutopilotIntent::DodgeObstacles) {
			Intent = AutopilotIntent::DodgeObstacles;
		}

		// since path is blocked, lets do or keep doing the double sphere scan
		scheduleDoubleSphereTraceNextTick = true;
	}

	if (doDoubleSphereTraceThisTick) {
		
		FVector possibleClearScanLocation; // may or may not be updated

		bool pathFound = doubleSphereTrace(from, to, scanRadius, ignoredActors, possibleClearScanLocation);

		if (pathFound) {
			// specify clear Y alignment
			LastClearPathScanLoc = possibleClearScanLocation;
		} else {
			++scanWidth; // increase scan width, 
		}
	}
}

void UAutoPilotV3::AutoScan() {

	FVector from = OwnerPhysicsComponent->GetComponentLocation();
	FVector to = from + FVector(ScanDistanceX, 0, 0);

	Scan(from, to, DefaultScanRadius, IgnoreList);
}

#pragma region Scan Utility Methods

TArray<FHitResult> UAutoPilotV3::filterHits(TArray<FHitResult> hits) {

	TArray<FHitResult> filtered;

	auto hasTags = [this](TArray<FName> tags) {

		for (auto t : TagsToIgnoreDuringPathFinding) {
			if (tags.Contains(t)) {
				return true;
			}
		}

		return false;
	};

	for (FHitResult h : hits) {

		bool actorShouldBeIgnored = hasTags(h.Actor->Tags);
		bool componentShouldBeIgnored = hasTags(h.Component->ComponentTags);

		if (actorShouldBeIgnored || componentShouldBeIgnored) {
			continue;
		} else {
			filtered.Add(h);
		}
	}

	return filtered;
}

TArray<FHitResult> UAutoPilotV3::sphereTrace(FVector from, FVector to, float sphereRadius, TArray<AActor*> ignoredActors) {

	// sanity check
	auto w = GetWorld();
	if (!w) {
		return TArray<FHitResult>();
	}

	TArray<AActor*> ignore;
	ignore.Append(ignoredActors);
	ignore.Add(GetOwner());

	TArray<FHitResult> hits;

	auto anything = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetOwner(),
		from,
		to,
		sphereRadius,
		DetectableObjectTypes,
		false,
		ignore,
		ShowDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		hits,
		true
	);

	// filter hits
	TArray<FHitResult> filteredHits = filterHits(hits);

	if (ShowDebug && anything) {

		for (FHitResult h : filteredHits) {

			FVector l = h.ImpactPoint;
			float weight = FMath::GetMappedRangeValueClamped(FVector2D(ScanDistanceX, 0.0), FVector2D(0.0, 1.0), h.Distance);
			DrawDebugPoint(w, l, DebugHitPointSize*weight, DebugHitPointColor, false, .5);

			if (ShowDebugExtra) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, h.Component->GetFullName());
			}
		}
	}

	return filteredHits;
}

bool UAutoPilotV3::doubleSphereTrace(FVector from, FVector to, float scanRadius, TArray<AActor*> ignoredActors, FVector& clearScanLocation) {

	FVector originL = from + FVector(0, scanRadius*scanWidth, 0);
	FVector originR = from - FVector(0, scanRadius*scanWidth, 0);

	FVector destinationL = originL + FVector(ScanDistanceX, 0, 0);
	FVector destinationR = originR + FVector(ScanDistanceX, 0, 0);

	auto traceL = sphereTrace(originL, destinationL, scanRadius, IgnoreList);
	auto traceR = sphereTrace(originR, destinationR, scanRadius, IgnoreList);

	auto hitCountL = traceL.Num();
	auto hitCountR = traceR.Num();

	if (hitCountL && hitCountR) { // if both sides are blocked
		
		return false;
	
	} else { // if one or both sides are clear

		if ((hitCountL + hitCountR) <= 0) { // both sides of the scan are clear!

			// pick the closer path of the two

			auto distA = UX::GetYDist(destinationL, from);
			auto distB = UX::GetYDist(destinationR, from);

			if (distA <= distB) {
				clearScanLocation = destinationL;
			} else {
				clearScanLocation = destinationR;
			}

		} else { // one side is clear one is blocked

			if (hitCountL <= 0) {
				clearScanLocation = destinationL;
			} else if (hitCountR <= 0) {
				clearScanLocation = destinationR;
			}
		}

		return true;
	}

	return false;
}

#pragma endregion