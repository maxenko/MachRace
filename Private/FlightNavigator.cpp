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
	for(int32 i = 0; i < scan.Num(); ++i){
		for (int32 n = 0; n < scan.Num(); ++n) {
			int32 indexDist = FMath::Abs(i - n);
			scan[n].Weight += scan[i].Distance / (indexDist + 1);
		}
	}

		// find max and min values
	float max = 0, min = FLT_MAX;
	for (auto& ray : scan) {
		if (ray.Weight > max) {
			max = ray.Weight;
		}

		if (ray.Weight < min) {
			min = ray.Weight;
		}
	}
		// normalize
	for (auto& ray : scan) {
		ray.Weight = FMath::GetMappedRangeValue(FVector2D(min, max), FVector2D(0, 1), ray.Weight);
	}

	// visualize ?
	if (DrawDebug) {
		for (auto& ray : scan) {
			DrawDebugLine(w, ray.From, ray.Hit.IsValidBlockingHit() ? ray.Hit.Location : ray.To, FColor::Purple, false, .04, 0, ray.Weight*2.0);
		}
	}

	return scan;
}


FVector UFlightNavigator::GetToLoc() {

	auto scan = getScan();

	// we are moving in -x as forward direction so, we are looking for Y value with highest weight
	FFlightNavigationRay maxRay;
	float max = FLT_MIN;
	for (auto& ray : scan) {
		if (ray.Weight > max) { 
			max = ray.Weight;
			maxRay = ray;
		}
	}

	// compose target from existing actor location, and heaviest ray
	FVector actorLoc = GetOwner()->GetActorLocation();
	FVector targetLoc = FVector(actorLoc.X, maxRay.From.Y, actorLoc.Z);

	return targetLoc;
}
