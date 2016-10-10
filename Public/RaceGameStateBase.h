// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/GameState.h"
#include "Engine/ExponentialHeightFog.h"
#include "CommonTypes.h"
#include "RaceContentInstancer.h"
#include "RaceGameStateBase.generated.h"

class ARacePlayerBase;

UCLASS()
class MACHRACE_API ARaceGameStateBase : public AGameState
{
	GENERATED_BODY()
	
private:
	void transitionAtmosphere(AExponentialHeightFog* fog, FAtmosphereSettings to, float speed);
	GameStage previousStage;
	FAtmosphereSettings atmosphereSettings;
	void SetAtmosphereByStage();
	

public:

	ARaceGameStateBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool IsInGame = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool ControlsEnabled = false;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	float TopSpeed = 0;

	UPROPERTY(BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool DisableObstacles = false;

	UPROPERTY(BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool DisableDecorativeGeometry = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	GameStage Stage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Grid")
	TArray<FVector> Level2Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Grid")
	int32 Level1Index = 0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Maintain State", Keywords = "Observe and maintain rules of the game."), Category = "MachRace|Gameplay")
	void MaintainState();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get next desert tile N", Keywords = "Gets next desert tile number based on Level1Index."), Category = "MachRace|Controls")
	int32 GetNextDesertTileN(bool increment);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Control settings", Keywords = "Get controls settings based on game stage and speed."), Category = "MachRace|Controls")
	FControlSettings GetControlSettings(float speed);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Camera Settings", Keywords = "Get camera settings based on ship speed and stage of the game."), Category = "MachRace|Presentation")
	FCameraSettings GetCameraSettings(float speed);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Mach Atmosphere Settings", Keywords = "Get atmosphere settings based on stage of the game."), Category = "MachRace|Presentation")
	void UpdateAtmosphereSettings();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get UnderFade Settings", Keywords = "Get ship underfade settings."), Category = "MachRace|Presentation")
	FUnderfadeSettings GetUnderFadeSettings();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Content Instancer", Keywords = "Get reference to content instancer."), Category = "MachRace|Engine")
	ARaceContentInstancer* GetInstancer(bool& success);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Exponential Fog", Keywords = "Get reference to content Exponential Fog component."), Category = "MachRace|Presentation")
	AExponentialHeightFog* GetExponentialFog(bool& success);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	ARaceContentInstancer* Instancer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	AExponentialHeightFog* ExponentialFog;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Reset XYZ Grid", Keywords = "Reset grid to current location of player."), Category = "MachRace|Engine")
	void ResetXYZGrid(float xOffset);

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System")
	ARacePlayerBase* Player;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Player", Keywords = "Get MachRace specific player."), Category = "MachRace|Engine")
	ARacePlayerBase* GetRacePlayer(bool& success);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Ship", Keywords = "Get MachRace specific ship."), Category = "MachRace|Engine")
	ARaceShipBase* GetRaceShip(bool& success);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Theoretical Speed Mult", Keywords = "Get theoretical speed multiplier, used to translate physical speed to theoretical."), Category = "MachRace|Engine")
	float GetTheoreticalSpeedMultiplier() {
		return 0.3176;
	}

};
