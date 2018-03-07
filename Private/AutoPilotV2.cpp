// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "X.h"
#include "Kismet/KismetMathLibrary.h"
#include "AutoPilotV2.h"


// Sets default values for this component's properties
UAutoPilotV2::UAutoPilotV2() {
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAutoPilotV2::BeginPlay() {
	Super::BeginPlay();
	root = GetOwner()->GetRootComponent();

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("RunScanSequence"));

	GetWorld()->GetTimerManager().SetTimer(ScanTimerHandle, TimerDelegate, ScanInterval, true);
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

	if (ShowDebug && anything) {

		for (FHitResult h : hits) {

			FVector l = h.ImpactPoint;
			float weight = FMath::GetMappedRangeValueClamped(FVector2D(ScanDistance, 0.0), FVector2D(0.0, 1.0), h.Distance);
			DrawDebugPoint(w, l, DebugHitPointSize*weight, DebugHitPointColor, false, .5);

		}
	}

	return hits;
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

bool UAutoPilotV2::shouldChase() {
	return (ChaseTarget && Target);
}

bool UAutoPilotV2::isStableY() {
	return FMath::IsNearlyZero(OwnerPhysicsComponent->GetPhysicsLinearVelocity().Y);
}

// calculates Y alignment velocity given current clear path vector (Y component is what we care about)
FVector UAutoPilotV2::calculateYAlignmentVelocity(FVector destination) {

	// dist to clear vector
	auto yDist = UX::GetYDist(ownerLoc, destination);
	auto dir = destination - ownerLoc; // non-normalized, we use the amplitude for velocity

	// calc decay multiplier, so when owner is closer to its ideal vector, it slows down, otherwise it jitters back and forth.
	auto decayMultiplier = FMath::GetMappedRangeValueClamped(FVector2D(0, ManuveringDecayRadius), FVector2D(0, 1), yDist);

	// alignment force in y
	auto yForce = FMath::Clamp(dir.Y * ManuveringAccelerationMultiplier, -MaximumManuveringSpeed, MaximumManuveringSpeed)*decayMultiplier;
	
	auto force = FVector(0, yForce, 0);

	if (ShowDebug && ShowDebugExtra) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Calculated Y velocity: dir y: %f, force y: %f"), dir.Y, force.Y));
		DrawDebugDirectionalArrow(GetWorld(), ownerLoc, ownerLoc + FVector(0, dir.Y, 0), 50, FColor::Green, false, 1, 0, 5);
	}

	return force;
}

FVector UAutoPilotV2::CalculateDesiredVelocity() {

	/*
	auto forwardVelocity	= FVector::ZeroVector;	// temporary placeholder
	auto dodgeVelocity		= FVector::ZeroVector;	// temporary placeholder
	auto followVelocity		= FVector::ZeroVector;	// temporary placeholder

	auto yDist = UX::GetYDist(ownerLoc, ClearPathVector);
	auto decayMultiplier = FMath::GetMappedRangeValueClamped(FVector2D(0, ManuveringDecayRadius), FVector2D(0, 1), yDist);

	//OwnerPhysicsComponent->SetAllPhysicsLinearVelocity()
	*/
	return FVector::ZeroVector;
}

void UAutoPilotV2::chase() {
	// nudge owner into Y alignment with Target

	Status = AutopilotStatus::Chasing;

	auto followVelocity = calculateYAlignmentVelocity(Target->GetActorLocation());
	auto currentVelocity = OwnerPhysicsComponent->GetPhysicsLinearVelocity();
	auto desiredVelocity = FMath::VInterpTo(currentVelocity, followVelocity, delta, ManuveringAdjustmentSpeed); // softly adjust into it

	OwnerPhysicsComponent->SetPhysicsLinearVelocity(desiredVelocity);
}

void UAutoPilotV2::manuver() {
	auto aligned = FMath::IsNearlyEqual(ownerLoc.Y, ClearPathVector.Y, AlignmentThreshold);

	// just because there is a clear path doesn't mean we're in alignment with the clear path!
	if (!aligned) { // not in alignment check

		auto dodgeVelocity = calculateYAlignmentVelocity(ClearPathVector);
		auto finalVelocity = dodgeVelocity; // temp

		auto currentVelocity = OwnerPhysicsComponent->GetPhysicsLinearVelocity();
		auto desiredVelocity = FMath::VInterpTo(currentVelocity, finalVelocity, delta, ManuveringAdjustmentSpeed); // softly adjust into it

		OwnerPhysicsComponent->SetPhysicsLinearVelocity(desiredVelocity);

	// otherwise we are in alignment, 
	} else if (aligned) {

		// so lets change state to idle, so autopilot can make other decisions next tick
		if (Status != AutopilotStatus::Idle && Status != AutopilotStatus::Chasing && aligned ) {

			Status = AutopilotStatus::Idle;
			OnStatusChange.Broadcast(Status);
		} 
	}
}

void UAutoPilotV2::Navigate() {

	// sanity check, do we have a physics component we can use?
	if (!OwnerPhysicsComponent) {

		if (ShowDebug) {
			// spew some error text here, so the dev knows
		}
		return;
	}

	// scan should be running concurrently, setting PathStatus

	if (PathStatus == AutopilotPathStatus::NoPath || PathStatus == AutopilotPathStatus::Path) {

		if (Status != AutopilotStatus::Manuvering) {
			Status = AutopilotStatus::Manuvering;
			OnStatusChange.Broadcast(Status);
		}

		manuver();

	} else if (PathStatus == AutopilotPathStatus::Clear) {

		if (Status != AutopilotStatus::Chasing) {
			Status = AutopilotStatus::Chasing;
			OnStatusChange.Broadcast(Status);
		}

		chase();

	}
}