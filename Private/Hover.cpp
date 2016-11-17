// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "Hover.h"


// Sets default values for this component's properties
UHover::UHover() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHover::BeginPlay() {
	Super::BeginPlay();

	// ...
}


// Called every frame
void UHover::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (!CallHoverManually) {
		Hover(DeltaTime);
	}
	
}

void UHover::Hover(float delta) {
	if (IsHoverEnabled) {

		FHitResult hit;
		checkGroundDist(hit);

		// non hex ground
		if (hit.IsValidBlockingHit()) {

			auto o = GetOwner();

			FVector actorLoc = o->GetActorLocation();
			FVector targetPos = FVector(actorLoc.X, actorLoc.Y, hit.Location.Z + MinDistFromGround);
			o->SetActorLocation(FMath::VInterpTo(actorLoc, targetPos, delta, GroundFollowSpeed));
		}
	}
}

float UHover::checkGroundDist(FHitResult& hit) {

	auto from = GetOwner()->GetActorLocation();
	auto to = from + FVector(0, 0, -100000);//FVector(from.X, from.Y, -100000);
	auto collisionParams = new FCollisionParameters();

	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());
	GetOwner()->GetWorld()->LineTraceSingleByChannel(hit, from, to, ECollisionChannel::ECC_Visibility,params);

	return FVector::Dist(from, hit.Location);
}

