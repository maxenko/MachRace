// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "LinearForwardScanner.h"

TArray<FHitResult> ULinearForwardScanner::Scan() {

	// do the trace

	return TArray<FHitResult>();
}

// creates the scan intervals, center out
TArray<float> ULinearForwardScanner::getIntervals() {

	float half = (DetectionRays*DetectionRayInterval) / 2.0f;

	TArray<float> intervals;
	float f = -half;

	for (int32 i = 0; i < DetectionRays; ++i) {
		intervals.Add(i * DetectionRayInterval + f + (DetectionRayInterval / 2));
	}

	return intervals;
}

TArray<FFlightNavigationRay> ULinearForwardScanner::getForwardScan() {

	// sanity check
	auto w = GetWorld();
	if (!w) {
		return TArray<FFlightNavigationRay>();
	}

	
	auto rayIntervals = getIntervals();					// relative origins for scan rays
	TArray<FFlightNavigationRay> scan;					// scan data
	scan.SetNum(DetectionRays);
	FTransform wT = GetOwner()->GetActorTransform();	// owner world Transform

	// reset roll a copy of owners Transform (otherwise we get rotation on scan, and we want it parallel to the ground, always)
	auto wR = wT.GetRotation();
	wR.X = 0;
	wT.SetRotation(wR);

	TArray<FVector> collisions;
	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());


	// generate scan
	for (int32 i = 0; i < DetectionRays; ++i) {

		scan[i].From = wT.GetLocation() + FVector(0, rayIntervals[i], 0);
		scan[i].To = wT.GetLocation() + FVector(-ScanDistance, rayIntervals[i], 0);

		FHitResult hit;
		w->LineTraceSingleByChannel(hit, scan[i].From, scan[i].To, ECollisionChannel::ECC_Visibility, params);

		if (hit.IsValidBlockingHit()) {

			collisions.Add(hit.Location);
			scan[i].Distance = FVector::Dist(hit.Location, scan[i].From);
			DetectedObstacle = true;

		} else {
			scan[i].Distance = ScanDistance;
		}

		scan[i].Hit = hit;
	}


	return TArray<FFlightNavigationRay>();
}