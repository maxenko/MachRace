// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "MachGameStateBase.h"
#include "HexTilePiece.generated.h"

USTRUCT(BlueprintType)
struct FHexTilePieceElementMeta
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Meta")
	int Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Meta")
	UClass* Type;
};


UCLASS()
class MACHRACE_API AHexTilePiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexTilePiece();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid")
	int32 Columns = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid")
	int32 Rows = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid")
	float CellSize = 120;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid")
	float PropagateThreshhold = 2000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid")
	TArray<FHexTilePieceElementMeta> WeightMap;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MachRace|Grid")
	UClass* GetRandomFromWeightMap();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MachRace|Grid")
	bool IsWithinThreshold(FVector to);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid")
	bool DidSpawnLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid")
	bool DidSpawnRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid")
	bool DidSpawnForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid", Meta = (ExposeOnSpawn = true))
	bool CanPropagate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Performance")
	bool DoDeferredDestroy = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "MachRace|Grid")
	float InstigatorScale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Grid")
	FVector GridIndex = FVector(0,0,0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "MachRace|Operation")
	FVector targetWorldLoc;

	UFUNCTION(BlueprintCallable, Category = "MachRace|Grid")
	void SpawnLeft(UClass* tile);

	UFUNCTION(BlueprintCallable, Category = "MachRace|Grid")
	void SpawnRight(UClass* tile);

	UFUNCTION(BlueprintCallable, Category = "MachRace|Grid")
	void SpawnForward(UClass* tile);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MachRace|Grid")
	FVector GetForwardTileSpawnPos();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MachRace|Grid")
	FVector GetLeftTileSpawnPos();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MachRace|Grid")
	FVector GetRightTileSpawnPos();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MachRace|Grid")
	AMachGameStateBase* const GetMachGameState();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void OnConstruction(const FTransform & Transform) override;

	virtual void PostInitProperties() override;

	virtual void PostInitializeComponents() override;


private:
	TArray<FHexTilePieceElementMeta> unwrappedWeightMap;
	
	bool registerIndex(FVector index);
	bool unRegisterIndex(FVector index);

	bool isInTheGrid(FVector v);

};
