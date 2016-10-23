// Copyright 2015 - Max Enko

#pragma once

#include "RaceActorBase.h"
#include "CommonTypes.h"
#include "HexTileBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAvailableIndexDelegate, FTransform, localT, FTransform, globalT);

UCLASS()
class MACHRACE_API AHexTileBase : public ARaceActorBase {
	GENERATED_BODY()
	
private:
	void projectGrid();
	bool isWithinThreshold(FVector v); // checks to see if two coordinates are within threshold specified by SpawnThreshold
	FVector cameraLoc;
	FVector cameraDir;
	static FHexTileDistribuition normilizeDistribution(FHexTileDistribuition d);

public:
	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Grid")
	TArray<FTransform> Grid;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Grid")
	TArray<FTransform> SpawnIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid")
	float HorisontalRange = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid")
	float VerticalRange = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid")
	float CellSize = 80;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float VisibleAngleThreshold = 40;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float DistanceThreshold = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Utility")
	bool DrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Utility")
	bool EnableSpawns = true;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|System")
	FAvailableIndexDelegate OnAvailableIndex;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Generate Grid Coordinates", Keywords = "Generate hexagonal grid transforms."), Category = "MachRace|Grid")
	void GenerateCoordinateGrid();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Scan Grid", Keywords = "Triggers event whenever a free index has entered the range."), Category = "MachRace|Grid")
	void ScanVisibleGrid(bool triggerSpawn);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Take Index", Keywords = "Marks current Transform as having been taken by something being spawned."), Category = "MachRace|Grid")
	void TakeIndex(FTransform localT);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is Taken", Keywords = "Checks wether existing transform has been taken."), Category = "MachRace|Grid")
	bool IndexTaken(FTransform localT);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Distribution Map", Keywords = "Generates hexagon geometry distribution map based on various state conditions."), Category = "MachRace|Gameplay")
	FHexTileDistribuition GenerateDistributionMap();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Chance Hex Spawn", ExpandEnumAsExecs = "Branches"), Category = "MachRace|Gameplay")
	void HexTileChanceSpawn(FHexTileDistribuition d, EHexTileChance& Branches);



};
