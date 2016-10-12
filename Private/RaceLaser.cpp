// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "RaceLaser.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ARaceLaser::ARaceLaser() {
	bIgnoresOriginShifting = false;
	PrimaryActorTick.bCanEverTick = true;
}

// always tracing ahead, regardless if laser is firing or not
// mainly for on/off states between firing as laser cools off and turns on
bool ARaceLaser::traceAhead() {

	auto w = GetWorld();

	if (!w) {
		return false;
	}

	auto state = GetState();
	if (!state) {
		return false;
	}

	FHitResult hit;
	ECollisionChannel channel = ECollisionChannel::ECC_Visibility;

	auto actorLoc = GetActorLocation();
	auto from = actorLoc + From;
	auto to = from + To;

	bool block;

	// handle autoaim logic (sphere trace, and closest IsAutomAimTarget of RaceActorBase
	if (state->EnableAutoaim) {

		// sphere trace
		TArray<TEnumAsByte<EObjectTypeQuery>> channels;
		TArray<AActor*> ignore;
		TArray<FHitResult> hits;
		UKismetSystemLibrary::SphereTraceMultiForObjects(this, from, to, 600, AutoAimQueryParams, false, ignore, EDrawDebugTrace::ForOneFrame, hits, true);


		DrawDebugLine(w, from, to, FColor::Green, false, 0.0, 0, 5);

		if (hits.Num() <= 0) {

			block = false;

		} else {

			// find the closest
			FHitResult lastClosest;
			float lastClosestDist = 999999;

			bool hitFound = false;

			for (auto h : hits) {

				auto dist = FVector::Dist(h.Location, GetActorLocation());

				// are we hitting a target that can be autoaimed?
				if (h.Actor.Get()->IsA(ARaceActorBase::StaticClass())) {

					auto target = Cast<ARaceActorBase>(h.Actor.Get());

					// only process autoaim targets
					if (!target->IsAutoAimTarget) {
						continue;
					}

					if (lastClosestDist > dist) {
						lastClosestDist = dist;
						lastClosest = h;
						hitFound = true;
					}
				}
			}

			// if we got something
			if (hitFound) {
				hit = lastClosest;
				block = true;
			}
		}

	// handle standard (aim straight) logic
	} else {
		block = w->LineTraceSingleByChannel(hit, from, to, channel);
	}

	//DrawDebugLine(w, from, to, FColor::Red, false, 0.0, 0, 5);

	// 
	if (IsFiring == false && previousIsFiring == true) {
		OnLaserEndFiring();
	} else if (IsFiring == true && previousIsFiring == false) {
		OnLaserStartFiring();
	}
	previousIsFiring = IsFiring;

	if (block && IsFiring) {
		LastHit = hit;

		OnLaserHit(hit);
		isHitting = true;
	} else {
		if (isHitting == true) {
			isHitting = false;
			OnLaserEndHit();
		}

		if (IsFiring) {
			OnLaserNoHit();
		}
	}

	return block;
}

// Called when the game starts or when spawned
void ARaceLaser::BeginPlay() {
	Super::BeginPlay();
	
}

// Called every frame
void ARaceLaser::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );
	
	traceAhead();
}

