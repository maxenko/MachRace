// Copyright 2015 - Max Enko

#include "RaceShipBase.h"
#include "MachRace.h"
//#include "CustomExtensions.h"
#include "X.h"
#include "Kismet/KismetMathLibrary.h"


ARaceShipBase::ARaceShipBase() {
	bIgnoresOriginShifting = false;
	PrimaryActorTick.bCanEverTick = true;
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

	// trigger mach	speed changes
	auto theoreticalSpeed = GetTheoreticalSpeed();
	auto mach = FMath::DivideAndRoundDown((int32)theoreticalSpeed, (int32)MACH_SPEED);

	if (previousMach != mach) {
		previousMach = mach;
		OnMachSpeedChange.Broadcast(mach);
	}

	// trigger overcharge update
	auto overchargeCount = FMath::DivideAndRoundDown((int32)OverchargeTotal, (int32)ShieldMaxHitPoints);

	if (previousOverchargeCount != overchargeCount) {
		previousOverchargeCount = overchargeCount;
		OnOverchargeAdded.Broadcast();
	}

	// hovering
	auto state = GetState();
	if (state) {

		auto stage = state->Stage;

		// maintain same Z position as ship has now
		if (MinDistFromGround == -1) {
			
			SetActorLocation(GetActorLocation());

		} else {

			if (stage == GameStage::Desert || stage == GameStage::DesertBoss) { // stages where ship hovers

				// interp to target distance from ground (which could be changing frame to frame)
				MinDistFromGroundCurrent = FMath::FInterpTo(MinDistFromGroundCurrent, MinDistFromGround, DeltaSeconds, ShipHoverRealignmentSpeed);

				FHitResult hit;
				CheckGroundDist(hit);

				if (hit.IsValidBlockingHit()) {

					if (hit.Actor->ActorHasTag("Floor")) { // only adjust to actual desert floor, not various objects on the floor

						FVector actorLoc = GetActorLocation();
						FVector targetPos = FVector(actorLoc.X, actorLoc.Y, hit.Location.Z + MinDistFromGroundCurrent);
						SetActorLocation(FMath::VInterpTo(actorLoc, targetPos, DeltaSeconds, GroundFollowSpeed));
					}
				}

			} else { // stages where ship doesn't hover (it readjusts back to default hover value)

				FVector actorLoc = GetActorLocation();
				MinDistFromGroundCurrent = FMath::FInterpTo(actorLoc.Z, MinDistFromGround, DeltaSeconds, ShipHoverRealignmentSpeed);

				// fade to target hover height from current Z
				FVector targetPos = FVector(actorLoc.X, actorLoc.Y, MinDistFromGroundCurrent);
				SetActorLocation(FMath::VInterpTo(actorLoc, targetPos, DeltaSeconds, GroundFollowSpeed));
			}
		}
	}
}


void ARaceShipBase::decayLateralMovement(float delta) {
	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);

	if (!rootOk) {
		return;
	}

	auto velocity = physVol->GetPhysicsLinearVelocity();
	auto newLaterSpeed = FMath::FInterpTo(velocity.Y, 0, delta, 5);
	velocity.Y = newLaterSpeed;
	physVol->SetPhysicsLinearVelocity(velocity);
	OnBank.Broadcast(0);
}


// todo: replace this with UX implementation
void ARaceShipBase::decayRotationToZero(float delta) {

	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);

	if (!rootOk) {
		return;
	}

	// stop all rotation
	if (physVol->GetPhysicsAngularVelocityInRadians() != FVector::ZeroVector) {
		physVol->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
	}

	physVol->SetWorldRotation(FMath::RInterpTo(physVol->GetComponentRotation(), FRotator(0, 0, 0), delta, 5));
}


// Called when the game starts or when spawned
void ARaceShipBase::BeginPlay() {
	Super::BeginPlay();
	MinDistFromGroundCurrent = MinDistFromGround;
}


bool ARaceShipBase::IsVelocityChangeFatal() {
	float current = UX::GetRootLinearVelocity(this).X;

	if (current <= PreviousXVelocity) { // we are accelerating in -X, everything is ok
		PreviousXVelocity = current;
		return false;
	}

	if (current > 0) { // ship is going in reverse, our forward is -X
		return true;
	}

	return false;
}


// todo: replace this with UX implementation
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


void ARaceShipBase::changeSpeed(float by) {

	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);

	if (rootOk) {
		auto v = physVol->GetPhysicsLinearVelocity();
		physVol->SetPhysicsLinearVelocity(v + FVector(by, 0, 0));
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unable to accelerate ship, physics volume not found."));
	}

	if (by < 0) { // -X is forward, so negative value is acceleration forward
		OnAccelerate.Broadcast(by);
	} else {
		OnDecelerate.Broadcast(by);
	}

	OnSpeedChange.Broadcast();
}


bool ARaceShipBase::level1IsShipOutOfBounds(AActor* tile) {
	FVector tileLoc = tile->GetActorLocation();
	FVector shipLoc = GetActorLocation();

	float distFromCenterOfTile = UX::GetYDist(tileLoc, shipLoc);

	if (distFromCenterOfTile > Level1Bound) {
		OnOutOfLevelBounds.Broadcast();
		return true;
	}

	return false;
}


void ARaceShipBase::Accelerate(float forwardVelocity) {
	changeSpeed(-FMath::Abs(forwardVelocity));

	// cosmetic triggers
	auto stage = GetState()->Stage;
	if (stage == GameStage::InfiniteHex) {
		OnAccelerateFlash.Broadcast(1.0);
	}
}


void ARaceShipBase::Decelerate(float forwardVelocity) {
	changeSpeed(FMath::Abs(forwardVelocity));

	// cosmetic triggers
	auto stage = GetState()->Stage;
	if (stage == GameStage::InfiniteHex) {
		OnDecelerationFlash.Broadcast(1.0);
	}
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

		// add lateral impulse (move in Y)
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

/*
FVector ARaceShipBase::Spin(FVector impulse, float maxX) {
	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);
	auto finalImpulse = FVector::ZeroVector;

	if (rootOk) {
		auto currentAngVelocity = physVol->GetPhysicsAngularVelocityInRadians();
		if (maxX == 0 || FMath::Abs(currentAngVelocity.X ) < maxX) {
			physVol->AddAngularImpulseInRadians(impulse, NAME_None, true);
		}
		finalImpulse = physVol->GetPhysicsAngularVelocityInRadians();
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Ships root component is not a physics object. Cannot spin ship."));
	}

	return finalImpulse;
}
*/

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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Ships root component is not a physics object. Cannot read theoretical banking speed."));
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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Ships root component is not a physics object. Cannot read actual lateral speed."));
	}

	return 0;
}


FVector ARaceShipBase::GetFuselageAngularImpulse() {
	bool rootOk = false;
	auto physVol = getRootAsPrimitive(rootOk);

	if (rootOk) {
		return physVol->GetPhysicsAngularVelocityInRadians();
	}

	return FVector::ZeroVector;
}

void ARaceShipBase::TriggerIgnition(bool onOff) {
	OnIgnition.Broadcast(onOff);
}

void ARaceShipBase::BroadcastExploded() {
	OnExploded.Broadcast();
}



void ARaceShipBase::SetShipSpeed(float speed) {
	UPrimitiveComponent* root = Cast<UPrimitiveComponent>(GetRootComponent());

	if (root && GetState()) {

		FVector newV = FVector(

			-speed / GetState()->GetTheoreticalSpeedMultiplier(),
			root->GetPhysicsAngularVelocityInRadians().Y,
			root->GetPhysicsAngularVelocityInRadians().Z

		);

		root->SetPhysicsLinearVelocity(newV, false);
	}
}

void ARaceShipBase::ChargeShield(float amount) {

	float plusCharge = (ShieldHitPoints + amount);

	// are we going to max out shield?
	if (plusCharge > ShieldMaxHitPoints) {

		// add overflow to overcharge
		ShieldHitPoints = ShieldMaxHitPoints;

		if (EnableOvercharge) {
			OverchargeTotal += (plusCharge - ShieldMaxHitPoints);
		}

	}
	else {
		ShieldHitPoints = plusCharge;
	}

	ShieldHitPoints = FMath::Clamp<float>(ShieldHitPoints + amount, 0, ShieldMaxHitPoints);

	OnShieldCharge.Broadcast();

	OnShieldActivity.Broadcast();
}

void ARaceShipBase::DepleteShield(float amount) {

	ShieldHitPoints = FMath::Clamp<float>(ShieldHitPoints - amount, 0, ShieldMaxHitPoints);

	OnShieldDeplete.Broadcast();

	if (ShieldHitPoints == 0) {
		OnShieldDepleted.Broadcast();
	}

	OnShieldActivity.Broadcast();
}

int32 ARaceShipBase::GetOverchargeCount() {
	float wholes = (OverchargeTotal - FMath::Fmod(OverchargeTotal, ShieldMaxHitPoints)) / ShieldMaxHitPoints;
	return (int32)wholes;
}

int32 ARaceShipBase::ResetOverchargeCount() {
	auto was = GetOverchargeCount();
	OverchargeTotal = 0;
	return was;
}

void ARaceShipBase::TakeDeltaDamage(float delta, float dmgAmount) {

	float dmg = delta * dmgAmount;

	DepleteShield(dmg);
}