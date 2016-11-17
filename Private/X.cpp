// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "X.h"

FVector UX::GetRootLinearVelocity(AActor* target) {
	auto targetRoot = Cast<UStaticMeshComponent>(target->GetRootComponent());
	if (!targetRoot) { return FVector::ZeroVector; }
	return targetRoot->GetPhysicsLinearVelocity();
}

void UX::SetRootLinearVelocity(AActor* target, FVector v, bool addTo) {

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, v.ToString());

	auto targetRoot = Cast<UStaticMeshComponent>(target->GetRootComponent());
	if (!targetRoot) { return; }

	if ( addTo ) {
		targetRoot->SetAllPhysicsLinearVelocity(v, true);
		
	} else {
		targetRoot->SetAllPhysicsLinearVelocity(v, false);
	}
	
}

FVector UX::NullifyY(FVector v) {
	return FVector(v.X, 0, v.Z);
}