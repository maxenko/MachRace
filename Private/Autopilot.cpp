// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "Autopilot.h"
#include "X.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
UAutopilot::UAutopilot() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAutopilot::BeginPlay() {
	Super::BeginPlay();

	// ...
}


// Called every frame
void UAutopilot::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	lastDelta = DeltaTime;
	UpdateVelocity();
}


// calculates target velocity based on all current settings, and from all applicable velocities
FVector UAutopilot::getTargetVelocity() {
	
	// get current target actor velocity velocity (zero vector if not following or no target)
	FVector currentTargetActorVelocity	= Target && FollowTarget ? UX::GetRootLinearVelocity(Target) : FVector::ZeroVector;
	FVector currentTargetActorVelocityN = currentTargetActorVelocity;
	currentTargetActorVelocityN.Normalize();

	FVector targetActorLoc	= Target->GetActorLocation();
	FVector ownerLoc		= GetOwner()->GetActorLocation();

	// factor in target offset location (this can be a moving location in front/behind/side of target)
	TargetFollowLocation = FVector(FollowOffset, 0 , 0) + targetActorLoc; // follow location is just an X axis offset location


	//////////////////////////////////////////////////////////////////////////
	// calculate follow velocity
	//////////////////////////////////////////////////////////////////////////
	if (FollowTarget) {
		
		// if offset location doesn't match, we need to adjust velocity slightly
		// to get to that location
		if (ownerLoc.X != TargetFollowLocation.X) {

			// calc dist in X, clamp to offset to keep division no more than 1.0
			float dist = FVector::Dist(FVector(ownerLoc.X, 0, 0), FVector(TargetFollowLocation.X, 0, 0));

			// calc percentage of distance away from target follow location
			float pctOfDistToTargetFollowLoc = FMath::Clamp<float>(dist/FMath::Abs(FollowOffset), 0, 1);

			// multiplier for additional (accelerated) follow speed
			float multiplier		= FollowOffsetApproachCurve ? FollowOffsetApproachCurve->GetFloatValue(pctOfDistToTargetFollowLoc) : pctOfDistToTargetFollowLoc;
			float multiplierMod		= TargetFollowLocation.X < ownerLoc.X ? 1 : -1;
			float additionalSpeed	= MaxFollowSpeed * multiplier * multiplierMod;

			TargetFollowVelocity	= FVector(additionalSpeed, 0, 0);
		}

	} else {
		TargetFollowVelocity = FVector::ZeroVector;
	}


	//////////////////////////////////////////////////////////////////////////
	// calculate velocity to move away from objects in immediate proximity (safe space)
	// (for instance other drones), this is always on, there is no setting to turn it off.
	// calculated off scanAroundHits, which should be updated infrequently, as its a sphere trace
	//////////////////////////////////////////////////////////////////////////

	if (scanAroundStale) { // if fresh scan
		// update velocity
		SafeSpaceVelocity = FVector::ZeroVector;
		// update velocity
		for (auto sch : scanAroundHits) {
			auto v = (GetOwner()->GetActorLocation() - sch.ImpactPoint);
			auto dist = FVector::Dist(GetOwner()->GetActorLocation(), sch.ImpactPoint);

			if (dist >= SelfDistanceTo) {
				continue;
			}

			v.Normalize();
			SafeSpaceVelocity += (v * MaxSelfDistancingSpeed);
		}

		scanAroundStale = false; // reset back to fresh scan, so this is ignored until next scan
	}


	//////////////////////////////////////////////////////////////////////////
	// calculate dodge velocity
	//////////////////////////////////////////////////////////////////////////
	if (ObstacleDetected) {
	
		DodgeVelocity = FVector::ZeroVector;
	
	}
	

	//////////////////////////////////////////////////////////////////////////
	// calculate velocity to position owner in line (Y axis) with Target 
	//////////////////////////////////////////////////////////////////////////
	if (!ObstacleDetected && AlignWithTarget) {

		if(ownerLoc.Y != targetActorLoc.Y){

			// calculate distance from target in Y axis
			FVector tYLoc = FVector(targetActorLoc.Y, 0, 0);
			FVector oYLoc = FVector(ownerLoc.Y, 0, 0);

			float yDist				= FVector::Dist(tYLoc, oYLoc);
			float alignSpeed		= FMath::Clamp<float>(yDist, 0, AlignWithTargetSpeed);
			float multMod			= tYLoc.Y < oYLoc.Y ? 1 : -1;
			float fallOffMultiplier = FMath::GetMappedRangeValueClamped(FVector2D(0, AlignWithTargetSpeedFaloff), FVector2D(.5, 1), yDist); // .5 to make alignment faster at the end, could be exposed as a param in the future

			AlignWithTargetVelocity = FVector(0, alignSpeed*multMod, 0);

		} else {
			AlignWithTargetVelocity = FVector::ZeroVector;
		}
	}
	
	return UX::GetRootLinearVelocity(Target) + TargetFollowVelocity + DodgeVelocity + AlignWithTargetVelocity + SafeSpaceVelocity;
}


void UAutopilot::AdjustVelocityToFollowTarget(float delta) {
	if (!Target) {
		return;
	}
	
	UX::SetRootLinearVelocity(GetOwner(), getTargetVelocity());
}


void UAutopilot::ScanAhead() {

}

// scans in the sphere around the owner, processing heavy, not meant to be run on each frame, use sparingly
// scanAroundVersion increments with each scan for comparison
void UAutopilot::ScanAround() {

	auto w = GetWorld();
	if (!w) {
		return;
	}

	// sphere trace around the owner
	FVector from	= GetOwner()->GetActorLocation() + FVector(0, 0, 10000);
	FVector to		= GetOwner()->GetActorLocation(); // move end up slights, so there is something to trace...

	TArray<AActor*> ignore;
	ignore.Add(GetOwner());

	TArray<FHitResult> hits;

	auto anything = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetOwner(),
		from,
		to,
		2000.0,
		ScanAroundObjectTypes,
		false,
		ignore,
		ShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		hits,
		true
	);

	if (ShowDebug && anything) {
		for (FHitResult h : hits) {
			FVector l = h.ImpactPoint;
			DrawDebugPoint(w, l, 50, FColor::Orange, false, .5);
		}
	}

	scanAroundHits = hits;

	scanAroundStale = true; // mark scan as stale, velocity will recalculate
}


void UAutopilot::UpdateVelocity() {

	TargetVelocity = getTargetVelocity();

	FVector newV = FMath::VInterpTo(UX::GetRootLinearVelocity(GetOwner()), TargetVelocity, lastDelta, VelocityChangeSpeed);

	UX::SetRootLinearVelocity(GetOwner(), newV);
}