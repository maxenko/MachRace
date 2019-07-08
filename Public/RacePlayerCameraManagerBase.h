// Copyright 2015 - Max Enko

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "RaceGameStateBase.h"
#include "RacePlayerCameraManagerBase.generated.h"

UCLASS()
class MACHRACE_API ARacePlayerCameraManagerBase : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ARacePlayerCameraManagerBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System")
	ARaceGameStateBase* State;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Camera", Keywords = "Update MachRace camera with gameplay specific settings."), Category = "Presentation|Camera")
	void UpdateRaceView(UCameraComponent* c, FTransform t, float fov, FVector targetHudScale, float speed, float bloomIntensity, float bloomThreshold);


};
