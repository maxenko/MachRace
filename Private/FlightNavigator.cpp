// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "FlightNavigator.h"


// Sets default values for this component's properties
UFlightNavigator::UFlightNavigator(){
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UFlightNavigator::BeginPlay(){
	Super::BeginPlay();

}


// Called every frame
void UFlightNavigator::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ){
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	DetectObstacles();

}



TArray<float> UFlightNavigator::getIntervals() {

	float half = (DetectionRays*DetectionRayInterval) / 2.0f;

	TArray<float> intervals;
	float f = -half;

	for (int32 i = 0; i < DetectionRays; ++i) {
		intervals.Add(i * DetectionRayInterval + f + (DetectionRayInterval / 2));
	}

	return intervals;
}

TArray<FFlightNavigationRay> UFlightNavigator::getScan() {
	auto intervals = getIntervals();
}


TArray<FVector> UFlightNavigator::DetectObstacles() {

	auto intervals = getIntervals();
	TArray<FVector> froms;
	TArray<FVector> tos;
	froms.SetNum(DetectionRays);
	tos.SetNum(DetectionRays);
	FTransform wT = GetOwner()->GetActorTransform();


	// generate ray cast locations
	int32 count = 0;
	for (float y : intervals) {

		FVector wFrom = wT.TransformPosition(FVector(0, y, 0));
		FVector wTo = wT.TransformPosition(FVector(-ScanDistance, y, 0));

		froms[count] = wFrom;
		tos[count] = wTo;

		++count;
	}

	TArray<FVector> collisions;
	auto w = GetWorld();
	if (w) {

		for (int32 i = 0; i < DetectionRays; ++i) {
			FHitResult hit;
			FCollisionQueryParams params;
			params.AddIgnoredActor(GetOwner());

			w->LineTraceSingleByChannel(hit, froms[i], tos[i], ECollisionChannel::ECC_Visibility, params);

			if (hit.IsValidBlockingHit()) {
				collisions.Add(hit.Location);
			}

			DrawDebugLine(w, froms[i], hit.IsValidBlockingHit() ? hit.Location : tos[i], FColor::Purple, false, .08, 0, 2.0);
		}
	}

	// cast rays
	return collisions;
}
