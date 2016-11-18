// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "FlightNavigator.h"
#include "X.h"

// Sets default values for this component's properties
UFlightNavigator::UFlightNavigator() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UFlightNavigator::BeginPlay(){
	Super::BeginPlay();
}

// Ca0lled every frame
void UFlightNavigator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	hasObstacle = false; // reset on each tick

	if (DodgeObstacles) {
		if (!dodge(DeltaTime)) {
			if (MoveInFrontOfTarget && Target) {
				moveInFrontOfTarget(DeltaTime, Target->GetActorLocation());
			}
		}
	}

	if (FollowTarget) {
		followTarget(DeltaTime);
	}
}

bool UFlightNavigator::dodge(float delta) {

	FVector to = GetToLoc();

	if(hasObstacle){
		moveInFrontOfTarget(delta, to);
		return true;
	}

	return false;
}

// creates the scan intervals, centered
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
				// show heaviest ray in red
				DrawDebugLine(GetWorld(), ray.From, ray.Hit.IsValidBlockingHit() ? ray.Hit.Location : ray.To, FColor::Red, false, .04, 0, ray.Weight*2.0);
			}else{
				DrawDebugLine(GetWorld(), ray.From, ray.Hit.IsValidBlockingHit() ? ray.Hit.Location : ray.To, FColor::Turquoise, false, .04, 0, ray.Weight*2.0);
			}
		}
	}
}

TArray<FFlightNavigationRay> UFlightNavigator::generateNoHitResult() {

	// no hit result
	noHitRay.From = aggregateWorldLocation;
	noHitRay.To = noHitRay.From + FVector(-ScanDistance, 0, 0);
	noHitRay.Weight = 1;
	noHitRay.Distance = ScanDistance;

	TArray<FFlightNavigationRay> noHitResult = { noHitRay };
	drawDebug(noHitResult);

	return noHitResult;
}

TArray<FFlightNavigationRay> UFlightNavigator::getForwardScan() {

	
	
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
		return generateNoHitResult();
	}

	// calculate weights, - assigns heaviest weight to a ray that is surrounded by most rays that either didn't hit anything or hit something farthest.
	for(int32 i = 0; i < scan.Num(); ++i){
		for (int32 n = 0; n < scan.Num(); ++n) {

			if (n == i) {
				continue; // don't calculate against itself
			}

			float fractionOfMaxDist = scan[n].Distance / ScanDistance;
			scan[i].Weight += fractionOfMaxDist/FMath::Abs(n-i); // divide fraction by [n] rays distance from current [i] ray 
																 // (note: this is not actual 3D world dist, its just placement in array distance.)
		}
	}

	// perform side scans
	bool rightHasObstacles = hasSideObstacle(1);
	bool leftHasObstacles = hasSideObstacle(-1);

	if (rightHasObstacles || leftHasObstacles) {
		return generateNoHitResult();
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

// here we check whether there is an object to the side of the owner based on its physics bounds
bool UFlightNavigator::hasSideObstacle(int32 dir) {
	auto w = GetWorld();

	if (!w) {
		return false;
	}

	// get collision bounds
	FVector o = FVector(0, 0, 0);
	FVector b = FVector(0, 0, 0);
	auto owner = GetOwner();
	owner->GetActorBounds(true,o,b);

	float scanDist = LateralScanDistance;
	bool debug = DrawDebug;

	auto checkTrace = [&w, &dir, &o, &b, &scanDist, &owner, &debug](FVector from) {

		FHitResult hit;
		FCollisionQueryParams params;
		params.AddIgnoredActor(owner);
		auto to = from + FVector(0, (scanDist+o.Y)*dir, 0);

		w->LineTraceSingleByChannel(hit, from, to, ECollisionChannel::ECC_Visibility, params);

		if (debug) {
			DrawDebugLine(w, from, hit.IsValidBlockingHit() ? hit.Location : to, hit.IsValidBlockingHit() ? FColor::Red : FColor::Blue, false, .04, 0, hit.IsValidBlockingHit() ? 2.0 : 1.0);
		}

		return hit.IsValidBlockingHit();
	};

	// cast fill rays (variant)
	for (int32 i = 0; i < SideDetectionRays+1; ++i) {
		auto posX = FMath::GetMappedRangeValue(FVector2D(0, SideDetectionRays), FVector2D(-b.X, b.X), i);
		bool isBlocked = checkTrace(o + FVector(posX, 0, 0));
		if (isBlocked) {
			return true;
		}
	}

	return false;
}

FVector UFlightNavigator::GetToLoc() {

	auto scan = getForwardScan();

	// we are moving in -x as forward direction so, we are looking for Y value with highest weight
	FFlightNavigationRay maxRay;
	float max = FLT_MIN;

	for (auto& ray : scan) {
		if (ray.Weight >= max) { 
			max = ray.Weight;
			maxRay = ray;
		}
	}

	// compose target from existing actor location, and heaviest ray
	FVector targetLoc = FVector(aggregateWorldLocation.X, maxRay.From.Y, aggregateWorldLocation.Z);
	return targetLoc;
}

void UFlightNavigator::followTarget(float delta) {

	if (!Target) {
		return;
	}

	FVector targetVelocity	= UX::GetRootLinearVelocity(Target);
	FVector ownerVelocity	= UX::GetRootLinearVelocity(GetOwner());

	UX::SetRootLinearVelocity(GetOwner(), FVector(targetVelocity.X, ownerVelocity.Y, ownerVelocity.Z));
}

void UFlightNavigator::moveInFrontOfTarget(float delta, FVector to) {

	if (!Target || hasObstacle) {
		return;
	}

	auto ownerLoc	= GetOwner()->GetActorLocation();
	auto targetLoc	= to;
	auto desiredLoc = FVector(ownerLoc.X, targetLoc.Y, ownerLoc.Z);
	auto direction	= (ownerLoc - desiredLoc).GetSafeNormal();
	auto dist		= FVector::Dist(ownerLoc, desiredLoc);

	FVector velocity = dist*-direction*BankingSpeedMultiplier;
	if (dist < 10) {
		velocity = UX::NullifyY(velocity);
	}

	UX::SetRootLinearVelocity(GetOwner(), velocity);
} 