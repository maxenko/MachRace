// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "RaceLaser.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"


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
	From = FromMarker ? FromMarker->GetComponentLocation() : actorLoc;
	To = From + Direction;
	/*
	auto from = actorLoc + From;
	auto to = from + To;
	*/

	bool block = true;

	//////////////////////////////////////////////////////////////////////////
	// handle autoaim logic (sphere trace, and closest IsAutomAimTarget of RaceActorBase
	//////////////////////////////////////////////////////////////////////////
	if (state->EnableAutoaim) {

		// sphere trace
		TArray<TEnumAsByte<EObjectTypeQuery>> channels;
		TArray<AActor*> ignore = GetState()->GetActorsIgnoredByLaserTrace();
		TArray<FHitResult> hits;
		UKismetSystemLibrary::SphereTraceMultiForObjects(
			this, 
			From,
			To, 
			AutoAimRadius, 
			AutoAimQueryParams, 
			false, 
			ignore, 
			EDrawDebugTrace::ForOneFrame, 
			hits, 
			true
		);		

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

					// calc angle between forward and hit vector
					auto hitVector = (From - To);
					auto straightVector = (From - (From + FVector(-10000, 0, 0)));
					hitVector.Normalize();
					straightVector.Normalize();

					auto angle = FVector::DotProduct(hitVector, straightVector);
					auto deg = UKismetMathLibrary::DegCos(angle);

					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Angle: %f"), deg));

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

	//////////////////////////////////////////////////////////////////////////
	// handle standard (aim straight) logic
	//////////////////////////////////////////////////////////////////////////
	} else {
		block = w->LineTraceSingleByChannel(hit, From, To, channel);
	}

	if (ShowDebug) {
		FColor traceColor = FColor::Green;
		if (IsFiring) {
			traceColor = FColor::Red;
		}
		DrawDebugLine(w, From, To, traceColor, false, 0.0, 0, 5);
	}

	if (IsFiring == false && previousIsFiring == true) { // prevents double triggering laser from different keys

		EndFiring.Broadcast();

	} else if (IsFiring == true && previousIsFiring == false) { // prevents double triggering laser from different keys

		StartFiring.Broadcast();
	}

	previousIsFiring = IsFiring;

	if (block && IsFiring) { // we hit something while firing!

		LastHit = hit;
		HasHit.Broadcast(hit);
		isHitting = true;

	} else {

		if (isHitting == true) {
			isHitting = false;
			HitEnded.Broadcast();
		}

		if (IsFiring) {
			NoHit.Broadcast();
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

