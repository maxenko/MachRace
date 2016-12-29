// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "X.h"

FVector UX::GetRootLinearVelocity(AActor* target) {
	auto targetRoot = Cast<UStaticMeshComponent>(target->GetRootComponent());
	if (!targetRoot) { return FVector::ZeroVector; }
	return targetRoot->GetPhysicsLinearVelocity();
}

FVector UX::GetRootAngularVelocity(AActor* target) {
	auto targetRoot = Cast<UStaticMeshComponent>(target->GetRootComponent());
	if (!targetRoot) { return FVector::ZeroVector; }
	return targetRoot->GetPhysicsAngularVelocity();
}

void UX::SetRootLinearVelocity(AActor* target, FVector v, bool addTo) {

	auto targetRoot = Cast<UStaticMeshComponent>(target->GetRootComponent());
	if (!targetRoot) { return; }

	targetRoot->SetAllPhysicsLinearVelocity(v, addTo);
}

void UX::SetRootAngularVelocity(AActor* target, FVector v, bool addTo) {

	auto targetRoot = Cast<UStaticMeshComponent>(target->GetRootComponent());
	if (!targetRoot) { return; }

	targetRoot->SetAllPhysicsAngularVelocity(v, addTo);
}

FVector UX::NullifyY(FVector v) {
	return FVector(v.X, 0, v.Z);
}