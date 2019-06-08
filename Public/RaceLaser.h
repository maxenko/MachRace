// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "RaceActorBase.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "GenericRaceLaser.h"
#include "RaceLaser.generated.h"


UCLASS()
class MACHRACE_API ARaceLaser : public ARaceActorBase
{
	GENERATED_BODY()

private:

	bool traceAhead();
	bool doAutoAimTrace();
	bool isAutoAimScanDue();
	bool isHitting = false;
	bool previousIsFiring = false;
	FDateTime lastAutoAimTraceTime = FDateTime::Now();
	ARaceActorBase* autoAimTarget = NULL;
	FHitResult lastAutoAimHit;
	void buildBeam();
	void updateBeam();
	bool beamExists = false;
	USplineMeshComponent* beamMesh = NULL;
	
public:	

	ARaceLaser();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	//////////////////////////////////////////////////////////////////////////
	// gameplay specific properties
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsFiring;

	UPROPERTY(BlueprintReadOnly)
	FVector DefaultDirection = FVector(-1, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Direction = FVector(-999999,0,0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float AutoAimRadius = 600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float LaserConeOfVision = 45.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	USceneComponent* FromMarker;


	//////////////////////////////////////////////////////////////////////////
	// presentation
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	FVector2D BeamFromScale = FVector2D(1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	FVector2D BeamToScale = FVector2D(.1, .1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	UStaticMesh* BeamBodyTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	UMaterialInterface* BeamMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	FLinearColor LaserColor = FLinearColor::Yellow;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Presentation")
	FRotator LaserLookAtRot = FRotator::ZeroRotator;


	//////////////////////////////////////////////////////////////////////////
	// gameplay events
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(BlueprintReadOnly)
	FHitResult LastHit;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FHitDelegate HasHit;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FFireDelegate HitEnded;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FFireDelegate NoHit;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FFireDelegate FireTriggered;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FFireDelegate StartFiring;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FFireDelegate EndFiring;


	//////////////////////////////////////////////////////////////////////////
	// system properties
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector From = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector To = From + Direction;

	UPROPERTY(BlueprintReadOnly, Instanced, Category = "MachRace|System")
	USplineComponent* BeamPath;

	UPROPERTY(BlueprintReadOnly, Instanced, Category = "MachRace|System")
	TArray<USplineMeshComponent*> BeamSegments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	TArray<TEnumAsByte<EObjectTypeQuery>> AutoAimQueryParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Dev")
	bool ShowDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	int32 AutoAimScanInterval = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	TArray<AActor*> CalculatedTargets;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Apply Calculated Damage", Keywords = "This function will apply damage to any CalculatedTargets (targets which are not damaged by ray-tracing but by calculating their positions instead). Returns position of the first target."), Category = "MachRace|Gameplay")
	bool ApplyCalculatedDamage(float baseDamage, TSubclassOf<UDamageType> dmgType, FVector& hitLoc);

	//////////////////////////////////////////////////////////////////////////
	// gameplay
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float CalculatedDamageYDist = 150;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Calculated Damage Amount", Keywords = "Gets calculated damage based on distance and faloff."), Category = "MachRace|Gameplay")
	float GetCalculatedDamageAmount(float effectiveRange, float falloff, float damage, float distance);

};
