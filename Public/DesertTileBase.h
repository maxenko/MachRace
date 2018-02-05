// Copyright 2015 - Max Enko

#pragma once

#include "RaceActorBase.h"
#include "DesertTileBase.generated.h"

/**
 * 
 */
UCLASS()
class MACHRACE_API ADesertTileBase : public ARaceActorBase {
	GENERATED_BODY()

public:

	ADesertTileBase();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "MachRace|System|Gameplay")
	DesertTileType TileType;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Instancer", Keywords = "Gets MachRace instancer, same as Sate->GetInstancer."), Category = "MachRace|Utility")
	ARaceContentInstancer* GetInstancer(bool& success);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Obstacle Count", Keywords = "Get obstacle count based on speed of the ship."), Category = "MachRace|System")
	int32 GetObstacleCount();
};
