// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "X.h"

FVector UX::GetRootLinearVelocity(AActor* target) {
	auto targetRoot = Cast<UStaticMeshComponent>(target->GetRootComponent());
	if (!targetRoot) { return FVector::ZeroVector; }
	return targetRoot->GetPhysicsLinearVelocity();
}

void UX::SetRootLinearVelocity(AActor* target, FVector v) {
	auto targetRoot = Cast<UStaticMeshComponent>(target->GetRootComponent());
	if (!targetRoot) { return; }
	targetRoot->SetPhysicsLinearVelocity(v);
}



