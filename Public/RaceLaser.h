// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "RaceActorBase.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "RaceLaser.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFireDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndFireDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitDelegate, FHitResult, hit);


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
	void buildBeamSpline();
	bool beamExists = false;
	USplineMeshComponent* beamMesh = NULL;
	
public:	

	ARaceLaser();

	//////////////////////////////////////////////////////////////////////////
	// gameplay specific properties
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsFiring;

	UPROPERTY(BlueprintReadOnly)
	FVector DefaultDirection = FVector(-1, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Direction = FVector(-50000,0,0);

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

	// Updates the mesh of the segment matching the index.
	UFUNCTION(BlueprintCallable, Category = "MachRace|System")
	void CreateBeam();


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

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

};
