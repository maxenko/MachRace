// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "LinearForwardScanner.h"

TArray<FFlightNavigationRay> ULinearForwardScanner::Scan() {

	// do the trace
	LastScan = getForwardScan();
	return LastScan;
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


	ObstacleDetected = false; // reset

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

	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());

	// generate scan
	for (int32 i = 0; i < DetectionRays; ++i) {

		scan[i].From = wT.GetLocation() + FVector(0, rayIntervals[i], 0);
		scan[i].To = wT.GetLocation() + FVector(-ScanDistance, rayIntervals[i], 0);

		FHitResult hit;
		w->LineTraceSingleByChannel(hit, scan[i].From, scan[i].To, ECollisionChannel::ECC_OverlapAll_Deprecated, params);

		if (hit.IsValidBlockingHit()) {

			scan[i].Distance = FVector::Dist(hit.Location, scan[i].From);
			ObstacleDetected = true;

		} else {
			scan[i].Distance = ScanDistance;
		}

		scan[i].Hit = hit;
	}

	// calculate weights, - assigns heaviest weight to a ray that is surrounded by most rays that either didn't hit anything or hit something farthest.
	for (int32 i = 0; i < scan.Num(); ++i) {
		for (int32 n = 0; n < scan.Num(); ++n) {

			if (n == i) {
				continue; // don't calculate against itself
			}

			float fractionOfMaxDist = scan[n].Distance / ScanDistance;
			scan[i].Weight += fractionOfMaxDist / (FMath::Abs(n - i)); // divide fraction by [n] rays distance from current [i] ray
																	   // (note: this is not actual 3D world dist, its numerical
																	   // distance between array indices.)
		}
	}

	return scan;
}