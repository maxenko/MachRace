// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "FlightNavigator.h"


// Sets default values for this component's properties
UFlightNavigator::UFlightNavigator(){
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UFlightNavigator::BeginPlay(){
	Super::BeginPlay();

}


// Called every frame
void UFlightNavigator::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ){
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	DetectObstacles();

}



TArray<float> UFlightNavigator::getIntervals() {

	float half = (DetectionRays*DetectionRayInterval) / 2.0f;

	TArray<float> intervals;
	float f = -half;

	for (int32 i = 0; i < DetectionRays; ++i) {
		intervals.Add(i * DetectionRayInterval + f + (DetectionRayInterval / 2));
	}

	return intervals;
}

TArray<FFlightNavigationRay> UFlightNavigator::getScan() {
	
	auto w = GetWorld();
	if (!w) {
		return TArray<FFlightNavigationRay>();
	}

	auto intervals = getIntervals();
	TArray<FFlightNavigationRay> scan;
	scan.SetNum(DetectionRays);
	FTransform wT =  GetOwner()->GetActorTransform();

	TArray<FVector> collisions;
	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());

	// generate scan
	for (int32 i = 0; i < DetectionRays; ++i) {
		scan[i].From = wT.TransformPosition(FVector(0, intervals[i], 0));
		scan[i].To = wT.TransformPosition(FVector(-ScanDistance, intervals[i], 0));

		FHitResult hit;
		w->LineTraceSingleByChannel(hit, scan[i].From, scan[i].To, ECollisionChannel::ECC_Visibility, params);
		if (hit.IsValidBlockingHit()) {
			collisions.Add(hit.Location);
			scan[i].Distance = FVector::Dist(hit.Location,scan[i].From);
		} else {
			scan[i].Distance = ScanDistance;
		}
		scan[i].Hit = hit;		
	}

	// calculate weights
	for (auto ray : scan) {
		float pctOfDist = ray.Distance / ScanDistance;
	
		// add weights of nearby rays, decreasing value added by distance from current ray

	}

	// visualize
	if (DrawDebug) {
		for (auto ray : scan) {
			DrawDebugLine(w, ray.From, ray.Hit.IsValidBlockingHit() ? ray.Hit.Location : ray.To, FColor::Purple, false, .08, 0, 2.0);
		}
	}

	return scan;
}


TArray<FVector> UFlightNavigator::DetectObstacles() {

	auto scan = getScan();

	return TArray<FVector>();
}
