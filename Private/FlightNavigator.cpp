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

void UFlightNavigator::drawDebug(TArray<FFlightNavigationRay> rays) {
	// visualize ?
	if (DrawDebug) {
		for (auto& ray : rays) {
			if (ray.Weight == 1.0) {
				DrawDebugLine(GetWorld(), ray.From, ray.Hit.IsValidBlockingHit() ? ray.Hit.Location : ray.To, FColor::Red, false, .04, 0, ray.Weight*2.0);
			}else{
				DrawDebugLine(GetWorld(), ray.From, ray.Hit.IsValidBlockingHit() ? ray.Hit.Location : ray.To, FColor::Turquoise, false, .04, 0, ray.Weight*2.0);
			}
		}
	}
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

	// reset roll (otherwise we get rotation on scan, and we want it parallel to the ground)
	auto wR = wT.GetRotation();
	wR.X = 0;
	wT.SetRotation(wR);

	TArray<FVector> collisions;
	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());

	auto hasObstacle = false;

	// generate scan
	for (int32 i = 0; i < DetectionRays; ++i) {

		scan[i].From = wT.TransformPosition(FVector(0, intervals[i], 0));
		scan[i].To = wT.TransformPosition(FVector(-ScanDistance, intervals[i], 0));

		FHitResult hit;
		w->LineTraceSingleByChannel(hit, scan[i].From, scan[i].To, ECollisionChannel::ECC_Visibility, params);
		if (hit.IsValidBlockingHit()) {
			collisions.Add(hit.Location);
			scan[i].Distance = FVector::Dist(hit.Location,scan[i].From);
			hasObstacle = true;
		} else {
			scan[i].Distance = ScanDistance;
		}
		scan[i].Hit = hit;		
	}

	if (!hasObstacle) {
		// no hit result
		noHitRay.From = GetOwner()->GetActorLocation();
		noHitRay.To = noHitRay.From + FVector(-ScanDistance,0,0);
		noHitRay.Weight = 1;
		noHitRay.Distance = ScanDistance;

		TArray<FFlightNavigationRay> noHitResult = { noHitRay };
		drawDebug(noHitResult);
		return noHitResult;
	}

	// calculate weights
	for(int32 i = 0; i < scan.Num(); ++i){
		for (int32 n = 0; n < scan.Num(); ++n) {

			if (n == i) {
				continue;
			}

			float fractionOfMaxDist = scan[n].Distance / ScanDistance;
			scan[i].Weight += fractionOfMaxDist/FMath::Abs(n-i);
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

	drawDebug(scan);

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
