// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "RaceShipBase.h"
#include "CustomExtensions.h"
#include "Kismet/KismetMathLibrary.h"

ARaceShipBase::ARaceShipBase() {
	bIgnoresOriginShifting = false;
	PrimaryActorTick.bCanEverTick = true;
}


void ARaceShipBase::decayLateralMovement(float delta) {
	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);

	if (!rootOk) {
		return;
	}

	auto velocity		= physVol->GetPhysicsLinearVelocity();
	auto newLaterSpeed	= FMath::FInterpTo(velocity.Y, 0, delta, 5);
	velocity.Y			= newLaterSpeed;
	physVol->SetPhysicsLinearVelocity(velocity);
	OnBank.Broadcast(0);
}


void ARaceShipBase::decayRotationToZero(float delta) {

	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);

	if (!rootOk) {
		return;
	}

	// stop all rotation
	if (physVol->GetPhysicsAngularVelocity() != FVector::ZeroVector) {
		physVol->SetPhysicsAngularVelocity(FVector::ZeroVector);
	}

	physVol->SetWorldRotation(FMath::RInterpTo(physVol->GetComponentRotation(), FRotator(0, 0, 0), delta, 5));
}


// Called when the game starts or when spawned
void ARaceShipBase::BeginPlay() {
	Super::BeginPlay();
}


// Called every frame
void ARaceShipBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	// decay lateral movement (EnableLateralDecay is usually true when player is not giving <> input)
	if (EnableLateralDecay) {
		decayLateralMovement(DeltaSeconds);
		decayRotationToZero(DeltaSeconds);
		
		// decay yaw
		auto existingRot = GetActorRotation();
		auto decayedRot = FRotator(existingRot.Pitch, 0, existingRot.Roll);
		auto w = GetWorld();
		if (w) {
			SetActorRotation(FMath::RInterpTo(existingRot, decayedRot, w->GetDeltaSeconds(), 5));
		}
	}


	// float over the ground softly

	// only if game is not in prelaunch stage (no floor)
	auto state = GetState();
	if(state && state->Stage != GameStage::Prelaunch ){

		FHitResult hit;
		CheckGroundDist(hit);

		// non hex ground
		if( state->Stage != GameStage::InfiniteHex ){

			FVector actorLoc = GetActorLocation();
			FVector targetPos = FVector(actorLoc.X, actorLoc.Y, hit.Location.Z + MinDistFromGround);
			SetActorLocation(FMath::VInterpTo(actorLoc, targetPos, DeltaSeconds, GroundFollowSpeed));

		// hex ground - will have gaps, so we ignore non-hits
		} else if( hit.IsValidBlockingHit() ) {

			FVector actorLoc = GetActorLocation();
			FVector targetPos = FVector(actorLoc.X, actorLoc.Y, hit.Location.Z + MinDistFromGround);
			SetActorLocation(FMath::VInterpTo(actorLoc, targetPos, DeltaSeconds, GroundFollowSpeed));
		}
	}
}


UPrimitiveComponent* ARaceShipBase::getRootAsPrimitive(bool& success) {

	auto rootAsPrimitive = Cast<UPrimitiveComponent>(RootComponent);

	if (rootAsPrimitive) {
		success = true;
		return rootAsPrimitive;
	} else {
		success = false;
	}

	return NULL;
}


void ARaceShipBase::Accelerate(float forwardVelocity) {
	
	auto absV = FMath::Abs(forwardVelocity);

	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);

	if (rootOk) {
		auto v = physVol->GetPhysicsLinearVelocity();
		physVol->SetPhysicsLinearVelocity(v + FVector(-absV,0,0));
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unable to accelerate ship, physics volume not found."));
	}

	OnAccelerate.Broadcast(forwardVelocity);
}


float ARaceShipBase::CheckGroundDist(FHitResult& hit) {

	auto from = GetActorLocation();
	auto to = FVector(from.X, from.Y, -100000);
	auto collisionParams = new FCollisionParameters();

	GetWorld()->LineTraceSingleByChannel(hit, from, to, ECollisionChannel::ECC_Visibility);

	return FVector::Dist(from,hit.Location);
}


void ARaceShipBase::Bank(FVector impulse) {

	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);

	if (rootOk) {

		// add lateral impulse
		physVol->AddImpulse(impulse,NAME_None,true);

		if (impulse.Y > 0) {
			OnBank.Broadcast(1);
		} else if (impulse.Y < 0) {
			OnBank.Broadcast(-1);
		} else {
			OnBank.Broadcast(0);
		}

	} else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unable to bank ship, physics volume not found."));
	}
}


float ARaceShipBase::Yaw(bool direction) {

	auto w = GetWorld();
	if (!w) {
		return 0;
	}

	// yaw the ship in the direction of travel

	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);

	auto currentRot = GetActorRotation();
	auto velocity = physVol->GetPhysicsLinearVelocity();

	if (direction) {
		physVol->SetWorldRotation(FMath::RInterpTo(currentRot, FRotator(currentRot.Pitch, -MaxBankingYaw, currentRot.Roll), w->GetDeltaSeconds(), MaxBankingYawSpeed));
	} else {
		physVol->SetWorldRotation(FMath::RInterpTo(currentRot, FRotator(currentRot.Pitch, MaxBankingYaw, currentRot.Roll), w->GetDeltaSeconds(), MaxBankingYawSpeed));
	}

	return GetActorRotation().Yaw;
}


FVector ARaceShipBase::Spin(FVector impulse, float maxX) {
	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);
	auto finalImpulse = FVector::ZeroVector;

	if (rootOk) {
		auto currentImpulse = physVol->GetPhysicsAngularVelocity();
		if (maxX == 0 || FMath::Abs( currentImpulse.X ) < maxX) {
			physVol->AddAngularImpulse(impulse, NAME_None, true);
		}
		finalImpulse = physVol->GetPhysicsAngularVelocity();
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Ships root component is not a physics object. Cannot spin ship."));
	}

	return finalImpulse;
}


float ARaceShipBase::GetTheoreticalSpeed() {

	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);

	if (rootOk) {
		auto velocity = FMath::Abs(physVol->GetPhysicsLinearVelocity().X);
		return velocity * GetState()->GetTheoreticalSpeedMultiplier();
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Ships root component is not a physics object. Cannot read theoretical speed."));
	}

	return 0;
}


float ARaceShipBase::GetTheoreticalBankingSpeed() {

	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);

	if (rootOk) {
		auto velocity = FMath::Abs(physVol->GetPhysicsLinearVelocity().Y);
		return velocity * GetState()->GetTheoreticalSpeedMultiplier();
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Ships root component is not a physics object. Cannot read theoretical speed."));
	}

	return 0;
}


float ARaceShipBase::GetSpeed() {
	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);

	if (rootOk) {
		auto velocity = physVol->GetPhysicsLinearVelocity().X;
		return velocity;
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Ships root component is not a physics object. Cannot read actual speed."));
	}

	return 0;
}


float ARaceShipBase::GetBankingSpeed() {

	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);

	if (rootOk) {
		auto velocity = physVol->GetPhysicsLinearVelocity().Y;
		return velocity;
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Ships root component is not a physics object. Cannot read actual  lateral speed."));
	}

	return 0;
}


FVector ARaceShipBase::GetFuselageAngularImpulse() {
	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);

	if (rootOk) {
		return physVol->GetPhysicsAngularVelocity();
	}

	return FVector::ZeroVector;
}