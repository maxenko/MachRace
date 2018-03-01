// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "X.h"
#include "Kismet/KismetMathLibrary.h"
#include "AutoPilotV2.h"


// Sets default values for this component's properties
UAutoPilotV2::UAutoPilotV2()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAutoPilotV2::BeginPlay(){
	Super::BeginPlay();
	root = GetOwner()->GetRootComponent();
}


// Called every frame
void UAutoPilotV2::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ownerLoc = GetOwner()->GetActorLocation();
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

	auto anything = UKismetSystemLibrary::SphereTraceMultiForObjects(
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


bool UAutoPilotV2::Scan() {

	if (scanWidth <= 0) {

		// do SINGLE column trace
		auto forwardScan = sphereTrace(ownerLoc, ownerLoc + FVector(-ScanDistance, 0, 0), ScanRadius, IgnoreList);
		if (forwardScan.Num()) {

			// forward is clear
			scanWidth = 0; // reset scan width for next scan
			Status = AutopilotStatus::Clear;

		} else {

			// increase scan width for next scan
			scanWidth++;
			Status = AutopilotStatus::Blocked;
		}

	} else {

		// do DOUBLE column trace

		FVector originA = ownerLoc + FVector(0, ScanRadius*scanWidth, 0);
		FVector originB = ownerLoc - FVector(0, ScanRadius*scanWidth, 0);

		FVector destinationA = originA + FVector(ScanDistance, 0, 0);
		FVector destinationB = originB + FVector(ScanDistance, 0, 0);

		auto traceA = sphereTrace(originA, destinationA, ScanRadius, IgnoreList);
		auto traceB = sphereTrace(originB, destinationB, ScanRadius, IgnoreList);

		auto hitCountA = traceA.Num();
		auto hitCountB = traceB.Num();

		// if both scans are blocked
		if (hitCountA && hitCountB) {

			scanWidth++;
			Status = AutopilotStatus::Blocked;
			return false;

		} else {

			// in case there is no hits on either scan (both paths are clear)
			if ( !(hitCountA + hitCountB) ) {

				// pick a random line forward out of the two
				auto chooseA = FMath::RandRange(0, 1);
				if (chooseA) {
					ClearPathVector = destinationA;
				} else {
					ClearPathVector = destinationB;
				}

			} else {
				if (!hitCountA) {
					ClearPathVector = destinationA;
				} else if (!hitCountB) {
					ClearPathVector = destinationB;
				}
			}
		}
	}

	Status = AutopilotStatus::Dodging;
	return true;

	/*

	auto currentLocation = GetOwner()->GetActorLocation();
	auto forward = sphereTrace(currentLocation, currentLocation + FVector(-ScanDistance, 0, 0), ScanRadius, IgnoreList);

	TArray<FHitResult> blockingHits;
	
	// test to see if any of the hits are within the SafetyRadius in XZ plane (forward plane), which would mean they are blocking the way
	for(auto hit : forward) {
		auto dist = UX::DistSansX(hit.ImpactPoint, currentLocation);

		if (dist > ScanRadius) {
			continue;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Skipping: %f"), dist));
		}

		blockingHits.Add(hit);
	}

	NavigateDirection = calculateNavigationDirection(blockingHits);

	return blockingHits;

	*/
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
