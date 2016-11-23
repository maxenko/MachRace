// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "VelocityTilt.h"
#include "X.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UVelocityTilt::UVelocityTilt()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVelocityTilt::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UVelocityTilt::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (!Target || !TiltComponent) {
		return;
	}

	if (!ActivateManually) {
		Tilt();
	}
}

void UVelocityTilt::Tilt() {
	// face the direction
	FVector v = UX::GetRootLinearVelocity(Target);
	FVector al = Target->GetActorLocation();
	FRotator r = UKismetMathLibrary::FindLookAtRotation(al, al + v);

	FRotator rn = r;
	rn.Normalize();

	FVector vn = v;
	vn.Normalize();

	auto roll = FMath::Clamp<float>(fmod(vn.Y*50.0, 180), -180, 180);
	r.Roll = roll;

	TiltComponent->SetWorldRotation(r);
}

