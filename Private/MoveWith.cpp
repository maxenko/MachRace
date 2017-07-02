// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "MoveWith.h"


// Sets default values for this component's properties
UMoveWith::UMoveWith()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMoveWith::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMoveWith::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (EnableFollow) {
		if(Target->IsValidLowLevel()){
			
			FVector loc = Target->GetActorLocation();
			
			auto owner = this->GetOwner();
			if (owner) {

				if (owner->IsValidLowLevel()) {
					
					FVector ownerLoc = owner->GetActorLocation();
					FVector desiredLoc = Target->GetActorLocation() + Offset;

					float currX = FMath::FInterpTo(ownerLoc.X, desiredLoc.X, DeltaTime, FollowSpeed.X);
					float currY = FMath::FInterpTo(ownerLoc.Y, desiredLoc.Y, DeltaTime, FollowSpeed.Y);
					float currZ = FMath::FInterpTo(ownerLoc.Z, desiredLoc.Z, DeltaTime, FollowSpeed.Z);

					FVector newLoc = FVector(currX, currY, currZ);

					owner->SetActorLocation(newLoc);

					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("x: %f, y: %f, z: %f"), currX, currY, currZ));

				}
			}
		}
	}
}

