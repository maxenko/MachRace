// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "Autopilot.h"
#include "X.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LinearForwardScanner.h"
#include "SideScanner.h"


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

	// record original rotation
	DefaultTransform = GetOwner()->GetActorTransform();
}


// Called every frame
void UAutopilot::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	lastDelta = DeltaTime;
	UpdateVelocity();

	if (RestoreAngularVelocityToZero){
		RestoreRotationToDefault = false; // turn it off, otherwise there might be race conflict
		decayRadialVelocity();
		
	}

	if (RestoreRotationToDefault) {

		bool rootOk = false;
		auto root = UX::GetRootAsPrimitive(GetOwner(), rootOk);

		if (rootOk) {
			FRotator current = root->GetComponentRotation();
			FRotator target = FMath::RInterpTo(current, FRotator(0, 0, 0), GetWorld()->GetDeltaSeconds(), 2);
			FVector currentV = root->GetComponentVelocity();

			root->SetWorldRotation(FQuat(FRotator(0,0,0))); // ideally this should be gradually adjusted to target via RInterp, but there is a bug in UE4 that kills physics.

			if (current == FRotator(0, 0, 0)) {
				RestoreRotationToDefault = false;
			}
		}
	}

	// check alignment, notify if its found or lost
	if (Target) {
		float yDist = UX::GetYDist(Target->GetActorLocation(), GetOwner()->GetActorLocation());

		if (yDist > AlignmentThreshold) {
			if (lastAlignmentStatus != 0) {
				lastAlignmentStatus = 0;
				OnAlignmentWithinThreshold.Broadcast(false);
			}
		} else if (yDist <= AlignmentThreshold) {
			if (lastAlignmentStatus != 1) {
				lastAlignmentStatus = 1;
				OnAlignmentWithinThreshold.Broadcast(true);
			}
		}
	}

}


void UAutopilot::decayRadialVelocity() {
	auto o = GetOwner();
	
	// get root component primitive
	bool rootOK = false;
	auto root = UX::GetRootAsPrimitive(o, rootOK);
	if (rootOK) {
		//root->get
	}

	FVector current = UX::GetRootAngularVelocity(o);

	if (current == FVector::ZeroVector) {
		RestoreAngularVelocityToZero = false;
		OnAngularVelocityRestoredToZero.Broadcast();
		return;
	}

	FVector target = FMath::VInterpTo(current, FVector::ZeroVector, lastDelta, 6);
	UX::SetRootAngularVelocity(o, target);
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
	// calculate follow velocity, if set, autopilot will always follow
	// velocity of the Target (fly along)
	//////////////////////////////////////////////////////////////////////////
	if (FollowTarget) {
		
		// if offset location doesn't match, we need to adjust velocity slightly
		// to get to that location
		if (ownerLoc.X != TargetFollowLocation.X) {

			// calc dist in X, clamp to offset to keep division no more than 1.0
			float dist = FVector::Dist(FVector(ownerLoc.X, 0, 0), FVector(TargetFollowLocation.X, 0, 0));

			// calc percentage of distance away from target follow location
			float pctOfDistToTargetFollowLoc	= FMath::Clamp<float>(dist/FMath::Abs(FollowOffset), 0, 1);
			float dir							= TargetFollowLocation.X < ownerLoc.X ? 1 : -1; // direction
			float additionalSpeed				= MaxFollowSpeed * pctOfDistToTargetFollowLoc * dir;

			TargetFollowVelocity				= FVector(additionalSpeed, 0, 0);
		}

	} else {
		TargetFollowVelocity = FVector::ZeroVector;
	}

	//////////////////////////////////////////////////////////////////////////
	// calculate dodge velocity, if this is applicable, 
	// it overrides anything below, because we dodge before we do anything else
	//////////////////////////////////////////////////////////////////////////
	if (ObstacleDetected) {

		DodgeVelocity = FVector::ZeroVector;

		if (ForwardScanner) {
			
			// get direction away from obstacle loc
			int32 yDir				= UX::GetYDirMult(ownerLoc, obstacleLoc);

			// get Y distance based multiplier
			float yDistToObstacle	= UX::GetYDist(ownerLoc, obstacleLoc);

			DodgeVelocity			= FVector(0, MaxDodgeVelocity * yDir, 0);
		}
	} else {
		DodgeVelocity				= FVector::ZeroVector;
	}

	//////////////////////////////////////////////////////////////////////////
	// calculate velocity to move away from objects in immediate proximity (safe space)
	// (for instance other drones), this is always on, there is no setting to turn it off.
	// calculated off scanAroundHits, which should be updated infrequently, as its a sphere trace
	//////////////////////////////////////////////////////////////////////////
	if (scanAroundStale) { // if fresh scan
		// update velocity
		SafeSpaceVelocity = FVector::ZeroVector;
		for (auto sch : scanAroundHits) {
			auto v		= (GetOwner()->GetActorLocation() - sch.ImpactPoint);
			auto dist	= FVector::Dist(GetOwner()->GetActorLocation(), sch.ImpactPoint);

			if (dist >= SelfDistanceTo) {
				continue; // far enough, don't add to velocity
			}

			v.Normalize();
			SafeSpaceVelocity += (v * MaxSelfDistancingSpeed);
		}

		SafeSpaceVelocity = SafeSpaceVelocity.GetClampedToMaxSize(MaxSelfDistancingSpeed);

		scanAroundStale = false; // reset back to fresh scan, so this is ignored until next scan
	}

	//////////////////////////////////////////////////////////////////////////
	// calculate velocity to position owner in line (Y axis) with Target 
	//////////////////////////////////////////////////////////////////////////
	if (!ObstacleDetected && AlignWithTarget) {

		if(ownerLoc.Y != targetActorLoc.Y){

			// calculate distance from target in Y axis

			float yDist				= UX::GetYDist(targetActorLoc, ownerLoc);		// FVector::Dist(tYLoc, oYLoc);
			float alignSpeed		= FMath::Clamp<float>(yDist, 0, AlignWithTargetSpeed);
			int32 yDir				= UX::GetYDirMult(targetActorLoc, ownerLoc);	// Y axis direction in which we should dodge (left or right)

			AlignWithTargetVelocity = FVector(0, alignSpeed*yDir, 0);

		} else {
			AlignWithTargetVelocity = FVector::ZeroVector;
		}
	}

	FVector combinedVelocity = UX::GetRootLinearVelocity(Target) + TargetFollowVelocity + DodgeVelocity + AlignWithTargetVelocity + SafeSpaceVelocity;
	
	// check if there are obstacles on the side we're going in, if there are,
	// don't crash into stuff on that side
	if ( (combinedVelocity.Y > 0 && ObstacleLeftDetected) || (combinedVelocity.Y < 0 && ObstacleRightDetected)) {

		combinedVelocity.Y = 0;
	}

	return combinedVelocity;
}


//////////////////////////////////////////////////////////////////////////
// Scans ahead of the Owner (-X), and stores a location to avoid
//////////////////////////////////////////////////////////////////////////
void UAutopilot::ScanAhead() {

	ObstacleDetected = false;

	if (ForwardScanner) {

		ULinearForwardScanner* forwardScanner = Cast<ULinearForwardScanner>(ForwardScanner);

		scanAheadHits = forwardScanner->Scan();
		ObstacleDetected = forwardScanner->ObstacleDetected;

		
		// did we hit anything?
		for (auto h : scanAheadHits) {
			if (h.Hit.IsValidBlockingHit()) {
				ObstacleDetected = true;
				break;
			}
		}

		if (!ObstacleDetected) {
			return;
		}

		// find the heaviest ray
		FFlightNavigationRay heaviest;
		heaviest.Weight = 0;
		for (auto r : scanAheadHits) {
			if (r.Weight > heaviest.Weight) {
				heaviest = r;
			}
		}

		// remember location of the hit (since scan doesn't run every frame)
		obstacleLoc = heaviest.Hit.Location;

		// show debug?
		if (ShowDebug) {

			DrawDebugSphere(GetWorld(), obstacleLoc, 400, 6, FColor::Black, true, 500, 0, 20);

			// draw line for each scan ray, indicate ray weight with thicker line
			for (auto h : scanAheadHits) {
				DrawDebugLine(GetWorld(), h.Hit.TraceStart, h.Hit.IsValidBlockingHit() ? h.Hit.Location : h.Hit.TraceEnd, FColor::Red, false, .04, 0, h.Weight*2.0);
			}
		}
	}
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

	scanAroundHits	= hits;
	scanAroundStale = true; // mark scan as stale, velocity will recalculate
}


void UAutopilot::ScanSides() {
	if (SideScanner) {
		
		scanSidesHits = SideScanner->Scan(); // store scan, this may be used for cosmetic gameplay elements - like visuals or sounds when passing close to something.

		// cast to side scanner (for left/right obstacle detection)
		USideScanner* sideScanner = Cast<USideScanner>(SideScanner);
		if (sideScanner) {
			ObstacleLeftDetected	= sideScanner->ObstacleOnTheLeft;
			ObstacleRightDetected	= sideScanner->ObstacleOnTheRight;
		}

	} else {
		ObstacleLeftDetected = ObstacleRightDetected = false;
	}
}


void UAutopilot::UpdateVelocity() {

	FVector currentVelocity = UX::GetRootLinearVelocity(GetOwner());
	TargetVelocity = getTargetVelocity();
	FVector newV = FMath::VInterpTo(currentVelocity, TargetVelocity, lastDelta, VelocityChangeSpeed);
	UX::SetRootLinearVelocity(GetOwner(), newV);
}