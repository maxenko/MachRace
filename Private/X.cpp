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

UPrimitiveComponent* UX::GetRootAsPrimitive(AActor* a, bool& success) {

	if (!a) {
		success = false;
		return NULL;
	}

	auto rootAsPrimitive = Cast<UPrimitiveComponent>(a->GetRootComponent());

	if (rootAsPrimitive) {
		success = true;
		return rootAsPrimitive;
	} else {
		success = false;
	}

	return NULL;
}

void UX::DecayRootRotToZero(AActor* a, float delta, float decaySpeed) {
	bool rootOk = false;
	auto physVol = UX::GetRootAsPrimitive(a,rootOk);

	if (!rootOk) {
		return;
	}

	// stop all rotation
	if (physVol->GetPhysicsAngularVelocity() != FVector::ZeroVector) {
		physVol->SetPhysicsAngularVelocity(FVector::ZeroVector);
	}

	physVol->SetWorldRotation(FMath::RInterpTo(physVol->GetComponentRotation(), FRotator(0, 0, 0), delta, decaySpeed),false,nullptr,ETeleportType::TeleportPhysics);
}