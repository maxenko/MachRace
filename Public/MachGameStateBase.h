// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/GameState.h"
#include "Engine/ExponentialHeightFog.h"
#include "CommonTypes.h"
#include "MachGameStateBase.generated.h"


/**
 * 
 */
UCLASS()
class MACHRACE_API AMachGameStateBase : public AGameState
{
	GENERATED_BODY()

private:
	void transitionAtmosphere(AExponentialHeightFog* fog, FAtmosphereSettings to, float speed);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	TArray<FVector> Level2Index;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Control settings", Keywords = "Get controls settings based on game stage and speed."), Category = "Utility|Controls")
	FControlSettings GetControlSettings(GameStage stage, float speed);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Update Mach Camera Settings", Keywords = "Get camera settings based on ship speed and stage of the game."), Category = "Utility|Presentation")
	FCameraSettings GetCameraSettings(GameStage stage, float speed);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Mach Atmosphere Settings", Keywords = "Get atmosphere settings based on stage of the game."), Category = "Utility|Presentation")
	FAtmosphereSettings UpdateAtmosphereSettings(GameStage stage, AExponentialHeightFog* fog);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get UnderFade Settings", Keywords = "Get ship underfade settings."), Category = "Utility|Presentation")
	FUnderfadeSettings GetUnderFadeSettings(GameStage stage);

};
