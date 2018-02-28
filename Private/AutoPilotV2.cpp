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
void UAutoPilotV2::BeginPlay()
{
	Super::BeginPlay();

	// get root component
	auto root = GetOwner()->GetRootComponent();

	// is root 
	
}


// Called every frame
void UAutoPilotV2::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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


TArray<FHitResult> UAutoPilotV2::Scan() {

	auto currentLocation = GetOwner()->GetActorLocation();
	auto forward = sphereTrace(currentLocation, currentLocation + FVector(-ScanDistance, 0, 0), ScanRadius, IgnoreList);

	TArray<FHitResult> blockingHits;
	
	// test to see if any of the hits are within the SafetyRadius in XZ plane (forward plane), which would mean they are blocking the way
	for(auto hit : forward) {
		auto dist = UX::DistSansX(hit.ImpactPoint, currentLocation);

		if (dist > SafetyRadius) {
			continue;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Skipping: %f"), dist));
		}

		blockingHits.Add(hit);
	}

	NavigateDirection = calculateNavigationDirection(blockingHits);

	return blockingHits;
}

FVector UAutoPilotV2::calculateNavigationDirection(TArray<FHitResult> blockingHits) {

	if (blockingHits.Num() <= 0) {
		return FVector::ZeroVector;
	}

	auto average = FVector::ZeroVector;

	for (auto h : blockingHits) {
		// adjust for distance, so further away targets have less effect on navigation than closer targets
		float weight = FMath::GetMappedRangeValueClamped(FVector2D(ScanDistance,0.0), FVector2D(0.0, 1.0), h.Distance);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Weight: %f"), weight));
		average += (weight * h.ImpactPoint);
	}

	// current owner actor loc
	auto loc = UX::NullifyX( GetOwner()->GetActorLocation() );
	auto avg = UX::NullifyX(average / blockingHits.Num());

	DrawDebugPoint(GetWorld(), average, 20.0, DebugHitPointColor, false, 3);

	auto dir = UKismetMathLibrary::FindLookAtRotation(avg, loc).Vector();

	return dir;
}
