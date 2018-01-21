// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "TentacleBase.generated.h"

UCLASS()
class MACHRACE_API ATentacleBase : public ARaceActorBase
{
	GENERATED_BODY()

private:

	// finds which segment of the path (first point of the segment) current 'fill' falls onto
	int32 findPathSegmentIdxByFill();

	// find dist of fill in the path
	float distByFill(float range);
	
public:	
	// Sets default values for this actor's properties
	ATentacleBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tentacle")
	int32 SplinePointCount = 3;

	/** Tail of the tentacle position. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tentacle", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float StartFill = 0;

	/** Head of the tentacle position. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tentacle", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float EndFill = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tentacle")
	FVector Start = FVector(0,0,0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tentacle")
	FVector End = FVector(1000, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tentacle")
	FVector Scale = FVector(100, 100, 1000);

	/** Vector curve offset, used to add irregularity to the otherwise straight trajectory. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Tentacle")
	UCurveVector* Trajectory;

	/** Path which the tentacle will travel */
	UPROPERTY(BlueprintReadOnly, Instanced, Category = "Tentacle")
	USplineComponent* Path;

	UPROPERTY(BlueprintReadOnly, Instanced, Category = "Tentacle")
	TArray<USplineMeshComponent*> Segments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tentacle")
	UStaticMesh* TentacleBodyTemplate;

	UFUNCTION(BlueprintCallable, Category = "Tentacle")
	void BuildTentacle();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tentacle|Geometry")
	FVector GetPointCoord(int32 idx);

	// Updates the mesh of the segment matching the index.
	UFUNCTION(BlueprintCallable, Category = "Tentacle")
	bool UpdateSegment(int32 idx);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

};
