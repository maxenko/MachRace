// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "SideScanner.h"
#include "X.h"

//////////////////////////////////////////////////////////////////////////
// Performs a box scan in the Y direction of the pawns velocity, returns
// single result of the closest object. ObstacleOnTheRight & ObstacleOnTheLeft
// are convenience properties that can be checked if obstracle is found.
//////////////////////////////////////////////////////////////////////////
TArray<FFlightNavigationRay> USideScanner::Scan() {

	auto o = GetOwner();
	auto w = GetWorld();

	ObstacleOnTheLeft = ObstacleOnTheRight = false; // reset detection flags

	if (!o || !w) {
		LastScan = emptyResult;
	}
	
	// find direction owner is going in (left +Y or right -Y)
	FVector dir = UX::GetRootLinearVelocity(o);
	
	if (dir.Y != 0) { // if owner velocity in Y axis is 0, we don't need to do this scan

		// set up trace params
		FCollisionShape shape = FCollisionShape::MakeBox(ScanBoxExtent);
		FHitResult hit;
		
		FCollisionObjectQueryParams objQueryParams;
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(o);
		queryParams.AddIgnoredActors(IgnoredActors);

		FVector ownerLoc	= o->GetActorLocation();
		int32 m				= dir.Y > 0 ? 1 : -1;					  // left or right?
		FVector from		= ownerLoc + ScanOriginOffset;
		FVector to			= from + FVector(0, SideScanDist * m, 0); // in the direction of Y velocity

		// trace
		bool anything = GetWorld()->SweepSingleByObjectType(hit, from, to, FQuat::Identity, objQueryParams, shape, queryParams);

		// did we hit anything?
		if (anything) {

			// wrap the hit result in Ray meta
			TArray<FFlightNavigationRay> result;
			FFlightNavigationRay ray;
			ray.From		= ownerLoc;
			ray.To			= hit.Location;
			ray.Weight		= 1;
			ray.Distance	= SideScanDist;
			ray.Hit			= hit;

			result.Add(ray);
			LastScan		= result;

			DrawDebugSphere(GetWorld(), hit.Location, 100, 6, FColor::Red, true, 60, 0, 20);

			ObstacleOnTheLeft	= m > 0;
			ObstacleOnTheRight	= m < 0;
		}

	} else {
		LastScan = emptyResult;
	}
	
	return LastScan;
}