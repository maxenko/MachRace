// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "X.h"
#include "Kismet/KismetMathLibrary.h"

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

bool UX::VectorsWithinAngle(FVector a, FVector b, float angle) {
	// calc angle between forward and hit vector

	a.Normalize();
	b.Normalize();

	auto angeToEachOther = FVector::DotProduct(a, b);
	auto deg = UKismetMathLibrary::DegCos(angeToEachOther);

	return deg < angle;
}

bool UX::IsWithinCameraView(APlayerCameraManager* cm, FVector to, float within) {

	if (!cm) {
		return false;
	}

	auto cameraLoc = cm->GetCameraLocation();
	auto cameraDir = cm->GetCameraRotation().Vector();
	cameraDir.Normalize();

	FVector directional = cameraLoc - to; // to target normal
	directional.Normalize();

	// angle between vectors (we're concerned whether or not vector falls inside the visible cone & dist
	float dProductAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(cameraDir, directional))); // angle to target

	return FMath::Abs(within) > FMath::Abs(180 - dProductAngle);
}

float UX::SnapToInterval(float interval, float v) {
	
	float multiplier = v < 0 ? -1 : 1;

	float absI = FMath::Abs(interval);
	float absV = FMath::Abs(v);

	if (absV < absI) {
		return 0;
	}

	float remainder = FMath::Fmod(absV, absI);
	return v - remainder * multiplier;
}

FVector UX::RandVecInRange(FVector min, FVector max) {

	auto X = FMath::FRandRange(min.X, max.X);
	auto Y = FMath::FRandRange(min.Y, max.Y);
	auto Z = FMath::FRandRange(min.Z, max.Z);

	return FVector(X, Y, Z);
}