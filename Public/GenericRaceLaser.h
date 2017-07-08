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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void buildBeam();
	void updateBeam();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//////////////////////////////////////////////////////////////////////////
	// system
	//////////////////////////////////////////////////////////////////////////
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsFiring;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	USceneComponent* FromMarker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	USceneComponent* ToMarker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector From = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Direction = FVector(-50000, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector To = From + Direction;

	//////////////////////////////////////////////////////////////////////////
	// visual parameters
	//////////////////////////////////////////////////////////////////////////

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
};
