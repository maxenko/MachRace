// Copyright 2015 - Max Enko

#pragma once

#include "RaceActorBase.h"
#include "HexTileBase.generated.h"

/**
 * 
 */
UCLASS()
class MACHRACE_API AHexTileBase : public ARaceActorBase {
	GENERATED_BODY()
	
private:
	void projectGrid();
	bool isWithinThreshold(FVector v); // checks to see if two coordinates are within threshold specified by SpawnThreshold
	void drawDebug();

public:
	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Grid")
	TArray<FTransform> Grid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid")
	float HorisontalRange = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid")
	float VerticalRange = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid")
	float CellSize = 80;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	AActor* Target = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector SpawnThreshold = FVector(500,1000,0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float VisibleAngleThreshold = 40;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float DistanceThreshold = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Utility")
	bool DrawDebug = false;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Generate Grid Coordinates", Keywords = "Generate hexagonal grid transforms."), Category = "MachRace|Grid")
	void GenerateCoordinateGrid();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Calculate distances to Target", Keywords = "Calculates distances to target from each Grid transform. Worldspace."), Category = "MachRace|Grid")
	TArray<float> CalcDistancesToTarget();

};
