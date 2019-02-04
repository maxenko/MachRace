// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "TurretBase.h"
//#include "ObjectBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATurretBase::ATurretBase() {
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* root = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(root);
}

// Called when the game starts or when spawned
void ATurretBase::BeginPlay() {
	Super::BeginPlay();

	TurretBaseOriginalTransform = TurretBase->GetRelativeTransform();
	TurretBodyOriginalTransform = TurretBody->GetRelativeTransform();
}

// Called every frame
void ATurretBase::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );

	if (EnableTracking) {

		TrackingAim = track(DeltaTime);

		if(EnableDecals) {
			auto total = fadeDecals(DeltaTime);
		}

	} else {
		if ( !FMath::IsNearlyEqual(returnToOriginalRotationDelta, 0.0f, 0.001f)) {
			returnToOriginalRotationDelta = returnToOriginalRotation(DeltaTime);
		}
	}
}

float ATurretBase::track(float delta) {

	bool found;
	auto loc = getTargetWorldPosition(found);
	float targetAcquired = 0; // return value: 1 is acquired, 0 means not

	if (found) {

		auto targetRot		= getTargetLookAtRot(loc);

		// calc fraction of progress
		auto targetYaw		= FMath::Abs(targetRot.Yaw);
		auto currentRotBase	= TurretBase->RelativeRotation;
		auto currentRotBody = TurretBody->RelativeRotation;
		auto currentYawAbs	= FMath::Abs(currentRotBase.Yaw);
		auto difference		= FMath::Abs(currentYawAbs - targetYaw);
		auto diffFraction	= FMath::GetMappedRangeValueClamped(FVector2D(0, 180), FVector2D(1, 0), difference);

		// rotate the parts
		auto rotBase = FMath::RInterpTo(currentRotBase, targetRot, delta, TrackingSpeed);
		rotateBaseToTarget(rotBase);

		auto rotBody = FMath::RInterpTo(currentRotBody, targetRot, delta, TrackingSpeed);
		rotateBodyToTarget(rotBody);

		targetAcquired = diffFraction;
	}

	return targetAcquired;
}

float ATurretBase::returnToOriginalRotation(float delta) {

	auto currentRotBase = TurretBase->RelativeRotation;
	auto rotBase = FMath::RInterpTo(currentRotBase, TurretBaseOriginalTransform.Rotator(), delta, TrackingSpeed);
	rotateBaseToTarget(rotBase);

	auto currentRotBody = TurretBody->RelativeRotation;
	auto rotBody = FMath::RInterpTo(currentRotBody, TurretBodyOriginalTransform.Rotator(), delta, TrackingSpeed);
	rotateBodyToTarget(rotBody);

	
	auto distBase = FVector::Dist(currentRotBase.Vector(), TurretBaseOriginalTransform.Rotator().Vector());
	auto distBody = FVector::Dist(currentRotBody.Vector(), TurretBodyOriginalTransform.Rotator().Vector());

	// return avg sum of overall progress, 0 means it has returned to original rotation
	return (distBase + distBody) / 2;
}

FRotator ATurretBase::getTargetLookAtRot(FVector targetLoc) {
	auto direction = -GetActorTransform().InverseTransformPosition(targetLoc);
	FRotator rot = FRotationMatrix::MakeFromX(direction).Rotator();
	return rot;
}

FVector ATurretBase::getTargetWorldPosition(bool& found) {

	// get world location from possible cases
	if(Target){

		// actor? get its root component first
		if (Target->GetClass()->IsChildOf(AActor::StaticClass())) {

			AActor* a	= Cast<AActor>(Target);
			found		= true;

			return a->GetRootComponent()->GetComponentLocation();
		}

		// everything else, treat it like a scene component
		else if (Target->GetClass()->IsChildOf(USceneComponent::StaticClass())) {

			USceneComponent* c = Cast<USceneComponent>(Target);
			found = true;

			return c->GetComponentLocation();
		}
	}

	// failed to track
	found = false;
	return FVector(0,0,0); 
}

void ATurretBase::rotateBaseToTarget(FRotator rot) {

	if ( TurretBase ) {
		FRotator baseRot;
		baseRot.Yaw		= rot.Yaw;
		baseRot.Pitch	= TurretBase->RelativeRotation.Pitch;
		baseRot.Roll	= TurretBase->RelativeRotation.Roll;

		TurretBase->SetRelativeRotation(baseRot);
	}
}


void ATurretBase::rotateBodyToTarget(FRotator rot) {
	if (TurretBody) {
		TurretBody->SetRelativeRotation(rot);
	}
}



TArray<FHitResult> ATurretBase::Fire() {

	// only fire if we have some projectile origins
	auto hits = TArray<FHitResult>();

	if (this->ProjectileOrigins.Num() <= 0) {
		return hits;
	}

	// trace ahead of each
	for (auto origin : ProjectileOrigins) {
		
		if (!origin) {
			continue;
		}

		// using x as direction
		auto from = origin->GetComponentLocation();
		auto fv	  = origin->GetForwardVector();
		auto to	  = (fv * FireRange) + from; // distance in front

		FHitResult hit;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		bool isAHit = GetWorld()->LineTraceSingleByChannel(hit, from, to, ECollisionChannel::ECC_Visibility, params);

		if (isAHit) {
			hits.Add(hit);

			if (EnableDecals) {
				SpawnTrailDecal(hit);
			}
		}

		if (DrawDebug) {
			auto previousColor	= debugTraceLineColor;
			debugTraceLineColor = FColor::Orange;
			debugTraceLineColor = previousColor;
		}
	}

	return hits;
}

#pragma optimize("", off)
int ATurretBase::fadeDecals(float delta) {

	for (int i = 0; i < decals.Num(); ++i ) {

		if (i < decals.Num()) { // check again because we shrink
			continue;
		}

		if (!IsValid(decals[i])) {
			decals.RemoveAt(i);
			decals.Shrink();
			continue;
		}

		auto newScale = decals[i]->GetComponentScale();
		newScale.Z = FMath::FInterpTo(newScale.Z, 0.0001, delta, TraceFadeSpeed);
		decals[i]->SetWorldScale3D(newScale);
	}

	return decals.Num();
}
#pragma optimize("", on)

void ATurretBase::SpawnTrailDecal(FHitResult hit) {

	if (!DecalMaterial) {
		return;
	}

	UMaterialInstanceDynamic* dynM = UMaterialInstanceDynamic::Create(DecalMaterial, this);

	// ignore initial startup position
	if (previousHit.ImpactPoint.X == 0 && previousHit.ImpactPoint.Y == 0 && previousHit.ImpactPoint.Z == 0) {
		previousHit = hit;
	}

	FRotator normalRot = FRotationMatrix::MakeFromY((previousHit.ImpactPoint - hit.ImpactPoint).GetSafeNormal()).Rotator();

	UDecalComponent* decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), dynM, FVector(25, 25, 25), (previousHit.ImpactPoint+hit.ImpactPoint)/2, normalRot);
	decal->SetFadeOut(0, 5, true);

	decals.Add(decal);

	auto dist = FVector::Dist(previousHit.ImpactPoint, hit.ImpactPoint);
	decal->DecalSize.Y = dist*1.01;

	previousHit = hit;
}