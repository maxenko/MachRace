// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "AHexTileGrid.generated.h"

UCLASS()
class MACHRACE_API AHexTileGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexTileGrid();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Cols", ExposeOnSpawn = true), Category = "Grid")
	int32 cols;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Rows", ExposeOnSpawn = true), Category = "Grid")
	int32 rows;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Cell Size Width", ExposeOnSpawn = true), Category = "Grid")
	float cellSizeW;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Cell Size Height", ExposeOnSpawn = true), Category = "Grid")
	float cellSizeH;

	TArray<FVector> GenerateSpawnCoordinates(int w, int h, float cellSize);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void SpawnMainSection(UClass* typeOf, const FTransform t);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	
};
