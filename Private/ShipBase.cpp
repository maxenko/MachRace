// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "ShipBase.h"


// Sets default values
AShipBase::AShipBase() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AShipBase::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AShipBase::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );
}

void AShipBase::SetShipSpeed(float speed) {
	UPrimitiveComponent* root = Cast<UPrimitiveComponent>(GetRootComponent());

	if (root && GetState() ) {

		FVector newV = FVector(
		
			-speed / GetState()->GetTheoreticalSpeedMultiplier(),
			root->GetPhysicsAngularVelocity().Y,
			root->GetPhysicsAngularVelocity().Z

		);
	
		root->SetPhysicsLinearVelocity(newV, false);
	}
}

void AShipBase::ChargeShield(float amount) {

	float plusCharge = (ShieldHitPoints + amount);

	// are we going to max out shield?
	if (plusCharge > ShieldMaxHitPoints) {

		// add overflow to overcharge
		ShieldHitPoints = ShieldMaxHitPoints;
		OverchargeTotal += (plusCharge - ShieldMaxHitPoints);

	} else {
		ShieldHitPoints = plusCharge;
	}

	ShieldHitPoints = FMath::Clamp<float>(ShieldHitPoints + amount, 0, ShieldMaxHitPoints);

	OnShieldCharge.Broadcast();

	OnShieldActivity.Broadcast();
}

void AShipBase::DepleteShield(float amount) {

	ShieldHitPoints = FMath::Clamp<float>(ShieldHitPoints - amount, 0, ShieldMaxHitPoints);

	OnShieldDeplete.Broadcast();

	if (ShieldHitPoints == 0) {
		OnShieldDepleted.Broadcast();
	}

	OnShieldActivity.Broadcast();
}

int32 AShipBase::GetOverchargeCount() {
	float wholes = (OverchargeTotal - FMath::Fmod(OverchargeTotal, ShieldMaxHitPoints))/ShieldMaxHitPoints;
	return (int32)wholes;
}