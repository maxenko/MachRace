// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "X.h"
#include "Algo/Reverse.h"
#include "Kismet/KismetMathLibrary.h"

FVector UX::GetRootLinearVelocity(AActor* target) {
	auto targetRoot = Cast<UStaticMeshComponent>(target->GetRootComponent());
	if (!targetRoot) { return FVector::ZeroVector; }
	return targetRoot->GetPhysicsLinearVelocity();
}

FVector UX::GetRootAngularVelocity(AActor* target) {
	auto targetRoot = Cast<UStaticMeshComponent>(target->GetRootComponent());
	if (!targetRoot) { return FVector::ZeroVector; }
	return targetRoot->GetPhysicsAngularVelocityInRadians();
}

void UX::SetRootLinearVelocity(AActor* target, FVector v, bool addTo) {

	auto targetRoot = Cast<UStaticMeshComponent>(target->GetRootComponent());
	if (!targetRoot) { return; }

	targetRoot->SetAllPhysicsLinearVelocity(v, addTo);
}

void UX::SetRootAngularVelocity(AActor* target, FVector v, bool addTo) {

	auto targetRoot = Cast<UStaticMeshComponent>(target->GetRootComponent());
	if (!targetRoot) { return; }

	targetRoot->SetAllPhysicsAngularVelocityInRadians(v, addTo);
}


FVector UX::NullifyX(FVector v) {
	return FVector(0, v.Y, v.Z);
}

FVector UX::NullifyY(FVector v) {
	return FVector(v.X, 0, v.Z);
}

FVector UX::NullifyZ(FVector v) {
	return FVector(v.X, v.Y, 0);
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
	if (physVol->GetPhysicsAngularVelocityInRadians() != FVector::ZeroVector) {
		physVol->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
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

FRotator UX::GetTargetLookAtRot(FTransform actorT, FVector targetLoc) {
	auto direction = -actorT.InverseTransformPosition(targetLoc);
	FRotator rot = FRotationMatrix::MakeFromX(direction).Rotator();
	return rot;
}

float UX::GetYDistBetweenActors(AActor* a, AActor* b) {

	 
	if (!a || !b) {
		return -1;
	}

	// strip out X,Z
	auto locA = FVector(0, a->GetActorLocation().Y, 0);
	auto locB = FVector(0, b->GetActorLocation().Y, 0);

	return FVector::Dist(locA, locB);
}

TArray<UObject*> UX::CullAtRandom(TArray<UObject*> a, int32 numberToCull) {

	// sanity check
	if (numberToCull == 0 || a.Num() == 0) {
		return a;
	}

	TArray<UObject*> b;
	b.Append(a);

	for (int32 i = 0; i < numberToCull; ++i) {

		if (b.Num() <= 0) {
			break;
		}

		b.RemoveAt(FMath::RandRange(0, b.Num() - 1));
	}

	return b;
}

float UX::PickRandomFloatFrom(TArray<float> floats) {
	int32 randIdx = FMath::RandRange(0, floats.Num() - 1);
	return floats[randIdx];
}

void UX::StoreIntSetting(FString category, FString settingName, int32 setting) {

	if (!GConfig) { return; }

	GConfig->SetInt(
		*category,
		*settingName,
		setting,
		GGameIni
	);
}

void UX::StoreStringSetting(FString category, FString settingName, FString setting) {

	if (!GConfig) { return; }

	GConfig->SetString(
		*category,
		*settingName,
		*setting,
		GGameIni
	);
}

int32 UX::GetIntSetting(FString category, FString settingName) {

	int32 setting = -1;

	GConfig->GetInt(
		*category,
		*settingName,
		setting,
		GGameIni
	);

	return setting;
}

FString UX::GetStringSetting(FString category, FString settingName) {

	FString setting = "";

	GConfig->GetString(
		*category,
		*settingName,
		setting,
		GGameIni
	);

	return setting;
}

float UX::DistSansX(FVector from, FVector to) {
	return FVector::Dist(FVector(0, from.Y, to.Z), FVector(0, to.Y, to.Z));
}