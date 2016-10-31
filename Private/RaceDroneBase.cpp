// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "RaceDroneBase.h"


TArray<float> ARaceDroneBase::getIntervals() {

	float half = (DetectionRays*DetectionRayInterval) / 2.0f;

	TArray<float> intervals;
	float f = -half;

	for (int32 i = 0; i < DetectionRays; ++i) {
		intervals.Add(i * DetectionRayInterval + f + (DetectionRayInterval/2));
	}

	return intervals;
}

TArray<FVector> ARaceDroneBase::DetectObstacles() {

	auto intervals = getIntervals();
	//auto forward = -GetActorForwardVector(); // -X
	TArray<FVector> froms;
	TArray<FVector> tos;
	froms.SetNum(DetectionRays);
	tos.SetNum(DetectionRays);
	FTransform wT = GetActorTransform();


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
			params.AddIgnoredActor(this);
			
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


// Called when the game starts or when spawned
void ARaceDroneBase::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void ARaceDroneBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	DetectObstacles();
}

