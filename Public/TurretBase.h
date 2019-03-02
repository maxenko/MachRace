// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "RaceActorBase.h"
#include "TurretBase.generated.h"

UCLASS()
class MACHRACE_API ATurretBase : public ARaceActorBase
{
	GENERATED_BODY()

private:

	FVector getTargetWorldPosition(bool& found);
	FRotator getTargetLookAtRot(FVector targetLoc);
	FColor debugTraceLineColor = FColor::Cyan;
	FHitResult previousHit = FHitResult(ForceInitToZero);
	TArray<UDecalComponent*> decals;

	void rotateBaseToTarget(FRotator rot);
	void rotateBodyToTarget(FRotator rot);
	
	float track(float delta);
	float returnToOriginalRotation(float delta);
	float returnToOriginalRotationDelta = 1.0f;

	int fadeDecals(float);

	FTransform TurretBaseOriginalTransform = FTransform::Identity;
	FTransform TurretBodyOriginalTransform = FTransform::Identity;


public:	
	// Sets default values for this actor's properties
	ATurretBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret|Operation")
	UObject* Target; // anything can be a target, so we keep it as generic as possible

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret|Setup")
	USceneComponent* TurretBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret|Setup")
	USceneComponent* TurretBody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret|Setup")
	TArray<USceneComponent*> ProjectileOrigins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret|Operation")
	float FireRange = 10000;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Fire", Keywords = "Fire raycasts at the world and return traced objects."), Category = "Turret|Operation")
	TArray<FHitResult> Fire();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret|Presentation")
	UMaterialInterface* DecalMaterial;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Spawn Decal", Keywords = "For line shaped tracemarks. Spawns decals in continuous trail (like laser trace)."), Category = "Turret|Presentation")
	void SpawnTrailDecal(FHitResult hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret|Operation")
	bool EnableTracking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret|Debug")
	bool DrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret|Operation")
	float TrackingSpeed = 1.0;

	UPROPERTY(BlueprintReadOnly, Category = "Turret|Operation")
	float TrackingAim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret|Presentation")
	float TraceFadeSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret|Presentation")
	bool EnableDecals = true;
};