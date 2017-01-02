// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/GameState.h"
#include "Engine/ExponentialHeightFog.h"
#include "CommonTypes.h"
#include "RaceContentInstancer.h"
#include "RaceGameStateBase.generated.h"

class ARacePlayerBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpawnLevel1Boss);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathLevel1Boss);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnEnemy, GameStage, stage);

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

	//////////////////////////////////////////////////////////////////////////
	// machrace system related
	//////////////////////////////////////////////////////////////////////////
	
	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System")
	TArray<AActor*> IgnoredByLaserTrace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	GameStage Stage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	GameStage PreviousStage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Grid")
	TArray<FVector> Level2Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Grid")
	int32 Level1Index = 0;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System|Gameplay")
	TArray<AActor*> ActiveEnemies;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Game Stage", Keywords = "Sets game stage, forces if necessary."), Category = "MachRace|System")
	void SetStage(GameStage newStage, bool force = false);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Reset XYZ Grid", Keywords = "Reset grid to current location of player."), Category = "MachRace|Engine")
	void ResetXYZGrid(float xOffset);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	ARaceContentInstancer* Instancer;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Content Instancer", Keywords = "Get reference to content instancer."), Category = "MachRace|Engine")
	ARaceContentInstancer* GetInstancer(bool& success);

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System")
	ARacePlayerBase* Player;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Player", Keywords = "Get MachRace specific player."), Category = "MachRace|Engine")
	ARacePlayerBase* GetRacePlayer(bool& success);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Ship", Keywords = "Get MachRace specific ship."), Category = "MachRace|Engine")
	ARaceShipBase* GetRaceShip(bool& success);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get next desert tile N", Keywords = "Gets next desert tile number based on Level1Index."), Category = "MachRace|Controls")
	int32 GetNextDesertTileN(bool increment);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add to Ignore By Laser", Keywords = "Add actor to registry of actors ignored by laser traces."), Category = "MachRace|System")
	void AddIgnoredByLaserTrace(AActor* actorToIgnore);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Active Enemy", Keywords = "Add actor to collections of enemies currently active in the level."), Category = "MachRace|Gameplay")
	void AddActiveEnemy(AActor* enemy);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Maintain State", Keywords = "Observe and maintain rules of the game."), Category = "MachRace|Gameplay")
	void MaintainState();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Control settings", Keywords = "Get controls settings based on game stage and speed."), Category = "MachRace|Controls")
	FControlSettings GetControlSettings(float speed);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Theoretical Speed Mult", Keywords = "Get theoretical speed multiplier, used to translate physical speed to theoretical."), Category = "MachRace|Engine")
	float GetTheoreticalSpeedMultiplier() {
		return 0.3176;
	}

	//////////////////////////////////////////////////////////////////////////
	// gameplay related 
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float Level1BossTriggerSpeed = 2600;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnSpawnLevel1Boss OnSpawnLevel1Boss;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnDeathLevel1Boss OnDeathLevel1Boss;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnSpawnEnemy OnSpawnEnemy;

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

	UPROPERTY(BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool DisableAccelerators = false;

	UPROPERTY(BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool EnableAutoAim = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool Level1BossDefeated = false;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Level 1 Boss as Defeated", Keywords = "Sets level one boss as defeated, affecting related state conditions."), Category = "MachRace|Gameplay")
	void SetLevelOneBossDeafeated();

	//////////////////////////////////////////////////////////////////////////
	// presentation 
	//////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Camera Settings", Keywords = "Get camera settings based on ship speed and stage of the game."), Category = "MachRace|Presentation")
	FCameraSettings GetCameraSettings(float speed);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Mach Atmosphere Settings", Keywords = "Get atmosphere settings based on stage of the game."), Category = "MachRace|Presentation")
	void UpdateAtmosphereSettings();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get UnderFade Settings", Keywords = "Get ship underfade settings."), Category = "MachRace|Presentation")
	FUnderfadeSettings GetUnderFadeSettings();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Exponential Fog", Keywords = "Get reference to content Exponential Fog component."), Category = "MachRace|Presentation")
	AExponentialHeightFog* GetExponentialFog(bool& success);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	AExponentialHeightFog* ExponentialFog;

};
