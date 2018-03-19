// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "X.h"
#include "Kismet/KismetMathLibrary.h"
#include "AutoPilotV2.h"

#pragma optimize("", off)
// Sets default values for this component's properties
UAutoPilotV2::UAutoPilotV2() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UAutoPilotV2::onOwnerHit() {
	wasJustHit = true;
}

// Called when the game starts
void UAutoPilotV2::BeginPlay() {
	Super::BeginPlay();
	root = GetOwner()->GetRootComponent();

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("RunScanSequence"));

	GetWorld()->GetTimerManager().SetTimer(ScanTimerHandle, TimerDelegate, ScanInterval, true);

	// bind to hit event on owners physics root
	if (OwnerPhysicsComponent) {
		FScriptDelegate hitDelegate;
		hitDelegate.BindUFunction(this, "onOwnerHit");
		OwnerPhysicsComponent->OnComponentHit.Add(hitDelegate);
	}
}

void UAutoPilotV2::EndPlay(const EEndPlayReason::Type reason) {
	ScanTimerHandle.Invalidate();

	Super::EndPlay(reason);
}


// Called every frame
void UAutoPilotV2::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	delta = DeltaTime;

	ownerLoc = GetOwner()->GetActorLocation();

	if (scanIsRunning) { // if we're scanning, don't navigate
		FindPath();
	}

	if (NavigateEachTick) {
		Navigate();
	}

}

#pragma region path finding  

TArray<FHitResult> UAutoPilotV2::filterHits(TArray<FHitResult> hits) {

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

TArray<FHitResult> UAutoPilotV2::sphereTrace(FVector from, FVector to, float sphereRadius, TArray<AActor*> ignoredActors) {

	// sanity check

	auto w = GetWorld();
	if (!w) {
		return TArray<FHitResult>();
	}

	TArray<AActor*> ignore;
	ignore.Append(ignoredActors);
	ignore.Add(GetOwner());

	TArray<FHitResult> hits;

	auto anything = UKismetSystemLibrary::SphereTraceMultiForObjects (
		GetOwner(),
		from,
		to,
		sphereRadius,
		DetectableObjectTypes,
		false,
		ignore,
		ShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		hits,
		true
	);


	// filter hits
	TArray<FHitResult> filteredHits = filterHits(hits);

	if (ShowDebug && anything) {

		for (FHitResult h : filteredHits) {

			FVector l = h.ImpactPoint;
			float weight = FMath::GetMappedRangeValueClamped(FVector2D(ScanDistance, 0.0), FVector2D(0.0, 1.0), h.Distance);
			DrawDebugPoint(w, l, DebugHitPointSize*weight, DebugHitPointColor, false, .5);

			if (ShowDebugExtra) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, h.Component->GetFullName());
			}
		}
	}

	return filteredHits;
}

int32 UAutoPilotV2::findPath_singleSphereTrace() {

	// do SINGLE column trace
	auto destination = ownerLoc + FVector(ScanDistance, 0, 0);
	auto forwardScan = sphereTrace(ownerLoc, destination, ScanRadius, IgnoreList);
	auto hitsN = forwardScan.Num();

	// no hits? then we're clear!
	if (hitsN <= 0) {
		ClearPathVector = destination;
	} 

	// path is blocked
	return hitsN;
}

int32 UAutoPilotV2::findPath_doubleSphereTrace(bool &pathFound) {
	// do DOUBLE column trace (one on the left one on the right at ScanRadius distance)

	FVector originA = ownerLoc + FVector(0, ScanRadius*scanWidth, 0);
	FVector originB = ownerLoc - FVector(0, ScanRadius*scanWidth, 0);

	FVector destinationA = originA + FVector(ScanDistance, 0, 0);
	FVector destinationB = originB + FVector(ScanDistance, 0, 0);

	auto traceA = sphereTrace(originA, destinationA, ScanRadius, IgnoreList);
	auto traceB = sphereTrace(originB, destinationB, ScanRadius, IgnoreList);

	auto hitCountA = traceA.Num();
	auto hitCountB = traceB.Num();

	// if both scans are blocked, we can't do much but keep scanning next frame with ++scanWidth
	if (hitCountA && hitCountB) {

		TArray<FHitResult> hits;

		hits.Append(traceA);
		hits.Append(traceB);
		OnObstaclesDetected.Broadcast(hits);

		scanWidth++;

		pathFound = false;

	} else {

		pathFound = true;

		// in case there is no hits on either scan (both paths are clear, we'll pick the closest one)
		if ( (hitCountA + hitCountB) <= 0) {

			// pick the closer path of the two
			auto distA = UX::GetYDist(destinationA, ownerLoc);
			auto distB = UX::GetYDist(destinationB, ownerLoc);

			if (distA <= distB) {
				ClearPathVector = destinationA;
			} else {
				ClearPathVector = destinationB;
			}

		// otherwise pick the clear one
		} else {

			if (hitCountA <= 0) {
				ClearPathVector = destinationA;
			} else if (hitCountB <= 0) {
				ClearPathVector = destinationB;
			}
		}

		// draw debug for clear vector
		if (ShowDebug) {
			DrawDebugDirectionalArrow(GetWorld(), FVector(ownerLoc.X, ClearPathVector.Y, ClearPathVector.Z), ClearPathVector, 300, FColor::Green, false, 1, 0, 10);
		}
	}

	return hitCountA + hitCountB;
}


bool UAutoPilotV2::FindPath() {
	
	// only mark scan as initiated first time its triggered (which is when scanInitiated is not true)
	if (!scanInitiated && !scanIsRunning) {
		scanInitiated = true;
	}

	scanIsRunning = true;

	// first Scan() run? Single sphere trace.
	if (scanWidth <= 0) {

		auto hitCount = findPath_singleSphereTrace();

		if (hitCount > 0) {
			scanWidth++; // next scan is double sphere trace

			if (PathStatus != AutopilotPathStatus::NoPath) {
				PathStatus = AutopilotPathStatus::NoPath;
				OnPathStatusChange.Broadcast(PathStatus);
			}

		} else {
			// path is clear
			resetScan();

			if (PathStatus != AutopilotPathStatus::Clear) {
				PathStatus = AutopilotPathStatus::Clear;
				OnPathStatusChange.Broadcast(PathStatus);
			}
		}
		
		return hitCount <= 0;

	// double Scan() run? Double sphere scan?
	} else {

		bool pathFound;
		auto hitCount = findPath_doubleSphereTrace(pathFound);

		if (!pathFound) {

			if (PathStatus != AutopilotPathStatus::NoPath) {
				PathStatus = AutopilotPathStatus::NoPath;
				OnPathStatusChange.Broadcast(PathStatus);
			}

			return false;

		} else {

			resetScan();
			if (PathStatus != AutopilotPathStatus::Path) {
				PathStatus = AutopilotPathStatus::Path;
				OnPathStatusChange.Broadcast(PathStatus);
			}

			return true;
		}
	}
}

void UAutoPilotV2::resetScan() {
	scanInitiated = false;
	scanIsRunning = false;
	scanWidth = 0;
}

FVector UAutoPilotV2::calculateNavigationDirection(TArray<FHitResult> blockingHits) {

	if (blockingHits.Num() <= 0) {
		return FVector::ZeroVector;
	}

	auto average = FVector::ZeroVector;

	for (auto h : blockingHits) {
		average += h.ImpactPoint / ((float)blockingHits.Num());
	}

	// current owner actor loc
	auto loc = UX::NullifyX( GetOwner()->GetActorLocation() );
	auto avg = UX::NullifyX( average );

	DrawDebugPoint(GetWorld(), average, 20.0, DebugAverageHitPointColor, false, 3);

	auto dir = UKismetMathLibrary::FindLookAtRotation(avg, loc).Vector();

	return dir;
}

void UAutoPilotV2::RunScanSequence() {
	if (scanInitiated) { // block new sequence if one is already running
		return;
	} else {
		FindPath();
	}
}

#pragma endregion  

#pragma region manuvering

bool UAutoPilotV2::shouldChase() {
	return (ChaseTarget && Target);
}

FVector UAutoPilotV2::decayAngularVelocity() {
	auto current = OwnerPhysicsComponent->GetPhysicsAngularVelocityInRadians();

	if (current == FVector::ZeroVector) {
		return FVector::ZeroVector;
	}

	return FMath::VInterpTo(current, FVector::ZeroVector, delta, 1.0f);
}

// calculates Y alignment velocity given current clear path vector (Y component is what we care about)
FVector UAutoPilotV2::calculateYAlignmentVelocity(FVector destination) {

	// dist to clear vector
	auto yDist = UX::GetYDist(ownerLoc, destination);
	auto dir = destination - ownerLoc; // non-normalized, we use the amplitude for velocity

	// calc decay multiplier, so when owner is closer to its ideal vector, it slows down, otherwise it jitters back and forth.
	auto decayMultiplier = FMath::GetMappedRangeValueClamped(FVector2D(0, ManuveringDecayRadius), FVector2D(0, 1), yDist);

	// alignment force in y
	auto yForce = FMath::Clamp(dir.Y * ManuveringAccelerationMultiplier, -MaximumManuveringPhysicsForce, MaximumManuveringPhysicsForce)*decayMultiplier;
	
	auto force = FVector(0, yForce, 0);

	if (ShowDebug && ShowDebugExtra) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Calculated Y velocity: dir y: %f, force y: %f"), dir.Y, force.Y));
		DrawDebugDirectionalArrow(GetWorld(), ownerLoc, ownerLoc + FVector(0, dir.Y, 0), 50, FColor::Green, false, 1, 0, 5);
	}

	return force;
}

FVector UAutoPilotV2::CalculateAmbientVelocity() {

	FVector followVelocity = FVector::ZeroVector;

	// calculate follow velocity
	if(Target && FollowTarget){

		auto desiredPosition = Target->GetActorLocation() + TargetFollowOffset;
		
		auto distX = UX::GetXDist(ownerLoc, desiredPosition);
		auto distZ = UX::GetZDist(ownerLoc, desiredPosition);

		auto accelerationMultiplierX = FMath::GetMappedRangeValueClamped(FVector2D(0, FollowTargetAccelerationDecayDistance), FVector2D(0, FollowTargetAccelerationFactor), distX); // figure out multiplier, so we can speed up or slow down
		auto accelerationMultiplierZ = FMath::GetMappedRangeValueClamped(FVector2D(0, FollowTargetAccelerationDecayDistance), FVector2D(0, FollowTargetAccelerationFactor), distZ); // figure out multiplier, so we can speed up or slow down
		
		// is target in front or behind? We'll use this to set direction of X velocity, to either catch up or slow down
		auto multX = desiredPosition.X <= ownerLoc.X ? -1 : 1;
		auto multZ = desiredPosition.Z <= ownerLoc.Z ? -1 : 1;

		auto targetVelocity = UX::GetRootLinearVelocity(Target);
		auto accelerationSpeedX = accelerationMultiplierX * MaxAdditionalAccelerationPhysicsSpeed;
		auto accelerationSpeedZ = accelerationMultiplierZ * MaxAdditionalAccelerationPhysicsSpeed;

		followVelocity = (targetVelocity + FVector((accelerationSpeedX * multX), 0, (accelerationSpeedZ * multZ)));
	}

	auto x = FMath::Clamp(followVelocity.X, -MaxFollowPhysicsSpeed, MaxFollowPhysicsSpeed);
	auto z = FMath::Clamp(followVelocity.Z, -MaxFollowPhysicsSpeed, MaxFollowPhysicsSpeed);

	return FVector(x, 0, z);
}


FVector UAutoPilotV2::calcChaseVelocity() {
	// nudge owner into Y alignment with Target

	Status = AutopilotStatus::Chasing;

	auto followVelocity = calculateYAlignmentVelocity(Target->GetActorLocation());
	auto currentVelocity = OwnerPhysicsComponent->GetPhysicsLinearVelocity();
	auto desiredVelocity = FMath::VInterpTo(currentVelocity, followVelocity, delta, ManuveringInterpSpeed); // softly adjust into it

	return desiredVelocity;
}

FVector UAutoPilotV2::calcManuverVelocity() {
	auto aligned = FMath::IsNearlyEqual(ownerLoc.Y, ClearPathVector.Y, AlignmentThreshold);
	FVector ret = FVector::ZeroVector;

	// not aligned to clear path?
	if (!aligned) {

		auto dodgeVelocity = calculateYAlignmentVelocity(ClearPathVector);
		auto finalVelocity = dodgeVelocity; // temp

		auto currentVelocity = OwnerPhysicsComponent->GetPhysicsLinearVelocity();
		auto desiredVelocity = FMath::VInterpTo(currentVelocity, finalVelocity, delta, ManuveringInterpSpeed); // softly adjust into it

		ret = desiredVelocity;

	// otherwise we are in alignment, 
	} else if (aligned) {

		// so lets change state to idle, so autopilot can make other decisions next tick
		if (Status != AutopilotStatus::Idle && Status != AutopilotStatus::Chasing && aligned ) {

			Status = AutopilotStatus::Idle;
			OnStatusChange.Broadcast(Status);
		}
	}

	return ret;
}

bool UAutoPilotV2::sideIsBlocked(Side side) {

	TArray<FHitResult> hits;
	FCollisionObjectQueryParams params1 = FCollisionObjectQueryParams::AllObjects;

	for (auto t : DetectableObjectTypes) {
		//params1.AddObjectTypesToQuery((ECollisionChannel)t.GetValue());
	}

	FCollisionShape shape;
	shape.SetBox(SideScanBoxExtent);
	FCollisionQueryParams params2;
	params2.AddIgnoredActor(GetOwner());
	params2.AddIgnoredActors(IgnoreList);


	if (side == Side::Left) {

		// ray cast left (Y)
		GetWorld()->SweepMultiByObjectType(hits, ownerLoc, ownerLoc + FVector(0, SideScanDistance, 0), FQuat::Identity, params1, shape, params2);

	} else if (side == Side::Right) {

		// ray cast right (-Y)
		GetWorld()->SweepMultiByObjectType(hits, ownerLoc, ownerLoc + FVector(0, -SideScanDistance, 0), FQuat::Identity, params1, shape, params2);

	}

	auto filteredHits = filterHits(hits);
	return filteredHits.Num() > 0;
}

void UAutoPilotV2::TestSomething() {
	auto left = sideIsBlocked(Side::Left);
	if (left) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "left is blocked");
	}
}

void UAutoPilotV2::setAngularImpulseAndRotationFlags() {

	if (wasJustHit) {
		decayAngularVelocityToZero = true;
	}

	if (OwnerPhysicsComponent->GetPhysicsAngularVelocityInRadians() == FVector::ZeroVector) {

		decayAngularVelocityToZero = false;
		wasJustHit = false;

		auto ownerRot = GetOwner()->GetActorRotation();
		auto e = !ownerRot.Equals(VisualOrientation, RestoreVisualOrientationNearTo);
		if (e) {
			restoreRotationToVisualOrientation = true;
		}
	}
}

#pragma endregion  

void UAutoPilotV2::Navigate() {

	// sanity check, do we have a physics component we can use?
	if (!OwnerPhysicsComponent) {

		if (ShowDebug) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf( TEXT("OwnerPhysicsComponent is not set on: %s's AutoPilot component. Autopilot cannot function without it."), *GetOwner()->GetName()));
		}
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// Calculate maneuver/chase velocity
	//////////////////////////////////////////////////////////////////////////
	
	FVector chaseOrManuverVelocity = FVector::ZeroVector;

	if (PathStatus == AutopilotPathStatus::NoPath || PathStatus == AutopilotPathStatus::Path) {

		if (Status != AutopilotStatus::Manuvering) {
			Status = AutopilotStatus::Manuvering;
			OnStatusChange.Broadcast(Status);
		}

		chaseOrManuverVelocity = calcManuverVelocity();

	} else if (PathStatus == AutopilotPathStatus::Clear) {

		if (Status != AutopilotStatus::Chasing) {
			Status = AutopilotStatus::Chasing;
			OnStatusChange.Broadcast(Status);
		}

		chaseOrManuverVelocity = calcChaseVelocity();
	}

	//////////////////////////////////////////////////////////////////////////
	// Set linear velocity
	//////////////////////////////////////////////////////////////////////////

	// calc ambient velocity (includes follow speed)
	auto ambient = CalculateAmbientVelocity();
	auto currentVelocity = OwnerPhysicsComponent->GetPhysicsLinearVelocity();

	auto desiredAmbientVelocity = FMath::VInterpTo(currentVelocity, ambient, delta, FollowSpeed);

	// add all velocities together
	auto aggregateLinearVelocity =
		desiredAmbientVelocity +					// follow velocity
		FVector(0, chaseOrManuverVelocity.Y, 0);	// we only care about X  component

	OwnerPhysicsComponent->SetAllPhysicsLinearVelocity(aggregateLinearVelocity);

	//////////////////////////////////////////////////////////////////////////
	// Set angular velocity
	//////////////////////////////////////////////////////////////////////////

	setAngularImpulseAndRotationFlags();

	if (decayAngularVelocityToZero) {
		OwnerPhysicsComponent->SetAllPhysicsAngularVelocityInRadians(decayAngularVelocity());
	}

	if (restoreRotationToVisualOrientation) {

		if (!hasBroadcast_CanRestoreOrientation) {
			OnCanRestoreOrientation.Broadcast();
			hasBroadcast_CanRestoreOrientation = true;
		} 
		
		if( GetOwner()->GetActorRotation().Equals(VisualOrientation, RestoreVisualOrientationNearTo)) {
		
			hasBroadcast_CanRestoreOrientation = false;
			restoreRotationToVisualOrientation = false;

			OnOrientationRestored.Broadcast();
		}
	}
}
#pragma optimize("", on)