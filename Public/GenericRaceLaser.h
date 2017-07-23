// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "GenericRaceLaser.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFireDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndFireDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitDelegate, FHitResult, hit);

UCLASS()
class MACHRACE_API AGenericRaceLaser : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGenericRaceLaser();

private:
	USplineMeshComponent* beamMesh = NULL;
	bool previousFireStatus = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void buildBeam();
	void updateBeam();
	void clearBeam();
	void drawDebug(bool onlyIfFiring);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//////////////////////////////////////////////////////////////////////////
	// system
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Debug")
	bool DrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Debug")
	bool OnlyWhenFiring = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsFiring = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	USceneComponent* FromMarker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	USceneComponent* ToMarker;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set laser destination", Keywords = "Set laser destination."), Category = "MachRace|Gameplay")
	void SetLaserDestination(FVector toLoc);

	//////////////////////////////////////////////////////////////////////////
	// visual parameters
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	bool StopOnBlock = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	FVector2D BeamFromScale = FVector2D(1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	FVector2D BeamToScale = FVector2D(.1, .1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	UStaticMesh* BeamBodyTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	UMaterialInterface* BeamMaterial;

	UPROPERTY(BlueprintReadOnly, Instanced, Category = "MachRace|System")
	USplineComponent* BeamPath;

	UPROPERTY(BlueprintReadOnly, Instanced, Category = "MachRace|System")
	TArray<USplineMeshComponent*> BeamSegments;

	//////////////////////////////////////////////////////////////////////////
	// gameplay events
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FFireDelegate StartFiring;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FFireDelegate EndFiring;
};
