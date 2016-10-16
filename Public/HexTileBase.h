// Copyright 2015 - Max Enko

#pragma once

#include "RaceActorBase.h"
#include "HexTileBase.generated.h"

/**
 * 
 */
UCLASS()
class MACHRACE_API AHexTileBase : public ARaceActorBase
{
	GENERATED_BODY()
	
private:
	void projectGrid();

public:
	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	TArray<FTransform> Grid;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	float HorisontalRange = 10000;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	float VerticalRange = 10000;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	float CellSize = 80;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Generate Grid Coordinates", Keywords = "Generate hexagonal grid transforms."), Category = "MachRace|Grid")
	void GenerateCoordinateGrid();

};
