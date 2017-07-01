// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "SpawnActorWhenInVisionConeBase.h"
#include "X.h"


// Sets default values
ASpawnActorWhenInVisionConeBase::ASpawnActorWhenInVisionConeBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawnActorWhenInVisionConeBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnActorWhenInVisionConeBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// check if camera manager is defined
	auto thisLoc = GetActorLocation();

	if (CameraManager) {

		if (FVector::Dist(thisLoc, CameraManager->GetCameraLocation()) <= TriggerDistance) {

			bool isInCone = UX::IsWithinCameraView(CameraManager, thisLoc, TriggerConeAngle);

			if (isInCone) {

				if (TriggerContinuously || TriggerCount <= 0) {
					OnTrigger.Broadcast();
					TriggerCount++;
				}

			}
		}
	}
}

