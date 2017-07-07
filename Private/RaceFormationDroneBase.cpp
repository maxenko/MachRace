// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "RaceFormationDroneBase.h"
#include "X.h"
#include "RaceShipBase.h"

// Sets default values
ARaceFormationDroneBase::ARaceFormationDroneBase(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARaceFormationDroneBase::BeginPlay(){
	Super::BeginPlay();

	// set up wobble
	auto wobbleFrequency = FMath::FRandRange(WobbleRandomRange.X, WobbleRandomRange.Y);
	GetWorldTimerManager().SetTimer(wobbleTimer, this, &ARaceFormationDroneBase::generateRandomOffset, wobbleFrequency, true, 2.0);

	// set up ship detection (which triggers firing at the ship)
	GetWorldTimerManager().SetTimer(scanForTargetTimer, this, &ARaceFormationDroneBase::scanForTarget, ScanInterval, true, 0.0);
}

void ARaceFormationDroneBase::moveTo(FVector to, float delta, FVector speed) {

	auto currentLoc = GetActorLocation();

	auto x = FMath::FInterpTo(currentLoc.X, to.X, delta, speed.X);
	auto y = FMath::FInterpTo(currentLoc.Y, to.Y, delta, speed.Y);
	auto z = FMath::FInterpTo(currentLoc.Z, to.Z, delta, speed.Z);

	SetActorLocation(FVector(x, y, z));
}

// Called every frame
void ARaceFormationDroneBase::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	if (abandoning) {
		FVector to = FVector::ZeroVector;
		auto currentLoc = GetActorLocation();

		if (!Position) {
			to = currentLoc + AbandonOffset;
		} else {
			to = Position->GetComponentLocation() + AbandonOffset;
		}

		moveTo(to, DeltaTime, AbandonSpeed);

	} else {

		if (Position) {
			auto offset = Wobble ? wobbleOffset : FVector::ZeroVector;
			moveTo(Position->GetComponentLocation() + offset, DeltaTime, FollowSpeed);
		}
	}
}

void  ARaceFormationDroneBase::AssignPosition(USceneComponent* position) {
	Position = position;
}

void ARaceFormationDroneBase::AbandonFormation() {
	abandoning = true;
}

void ARaceFormationDroneBase::generateRandomOffset() {
	if (Wobble) {
		wobbleOffset = UX::RandVecInRange(WobbleOffsetMin, WobbleOffsetMax);
	}
}

//////////////////////////////////////////////////////////////////////////
// multi-tiered scan, which looks to see if there is nothing in front of drone
// and in that case, tries to see if race ship is in front of the drone (without trace),
// if ship is in front of drone, triggers targeted event, which is then handled in BP
//////////////////////////////////////////////////////////////////////////
void ARaceFormationDroneBase::scanForTarget() {

	auto w = GetWorld();
	if (!w) {
		return;
	}

	// scan for anything in front of the drone (+x)
	FHitResult hit;
	bool trace = w->LineTraceSingleByChannel(hit, GetActorLocation(), GetActorLocation()+FVector(10000,0,0), ECollisionChannel::ECC_Visibility);
	bool isHit = hit.IsValidBlockingHit();
	bool isHitOnFormationDrone = false;
	bool targetAquired = false;

	// if we hit ship
	if (isHit) {

		if (hit.Actor->IsA(ARaceFormationDroneBase::StaticClass())) {
			isHitOnFormationDrone = true;
		}

	// look for ship, without trace, just by its general location
	} else {

		auto state = GetState();

		// fires the event when this drone is ready to fire on the target (firing is handled in BP)
		auto acquireTarget = [&](AActor* ship) {
			FVector shipLoc = ship->GetActorLocation();
			FVector droneLoc = GetActorLocation();

			bool InYRange = FMath::IsNearlyEqual(shipLoc.Y, droneLoc.Y, FireTriggerYDist);
			bool InZRange = FMath::IsNearlyEqual(shipLoc.Z, droneLoc.Z, FireTriggerZDist);

			if (InYRange && InZRange && !isHitOnFormationDrone) {

				OnTargetAcquired.Broadcast(shipLoc, droneLoc);
				targetAquired = true;
				previousTargetStatus = true;

			} else {

				// inform BP of lost target
				if (previousTargetStatus) {
					previousTargetStatus = false;
					OnTargetLost.Broadcast();
				}
				targetAquired = false;
			}
		};

		// do we have race state?
		if (state) {
			
			bool gotShip = false;
			auto ship = state->GetRaceShip(gotShip);
			
			if (gotShip) {
				acquireTarget(ship);
			}

		// if we don't
		} else {

			// are we in test level?
			TArray<AActor*> ships;

			// find first race ship, and use it
			UGameplayStatics::GetAllActorsOfClass(w, ARaceShipBase::StaticClass(), ships);
			if (ships.Num() > 0) {
				// grab the first one
				auto ship = ships[0];
				acquireTarget(ship);
			}
		}
	}

	// draw debug data
	if (DrawDebug) {
		DrawDebugLine(w, 
			hit.TraceStart, 
			isHit ? hit.ImpactPoint : hit.TraceEnd,
			isHit ? (targetAquired ? FColor::Green : FColor::Red) : FColor::Blue,
			false, .8, 0, 5);
	}
}