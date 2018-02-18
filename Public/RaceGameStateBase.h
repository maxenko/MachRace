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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevel2Boss);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevel3Reached);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevel3Boss);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevel4Reached);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnEnemy, GameStage, stage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIntroduceDrone);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System|Grid")
	int32 Level2Count = 0;

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

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get next desert tile N", Keywords = "Gets next desert tile number based on Level1Index."), Category = "MachRace|Controls")
	int32 CountHexTile() {
		Level2Count++; 
		return Level2Count;
	}

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add to Ignore By Laser", Keywords = "Add actor to registry of actors ignored by laser traces."), Category = "MachRace|System")
	void AddIgnoredByLaserTrace(AActor* actorToIgnore);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Active Enemy", Keywords = "Add actor to collection of enemies currently active in the level."), Category = "MachRace|Gameplay")
	void AddActiveEnemy(AActor* enemy);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Remove Active Enemy", Keywords = "Removes actor from collection of enemies currently active in the level."), Category = "MachRace|Gameplay")
	bool RemoveActiveEnemy(AActor* enemy);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Maintain State", Keywords = "Observe and maintain rules of the game."), Category = "MachRace|Gameplay")
	void MaintainState();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Control settings", Keywords = "Get controls settings based on game stage and speed."), Category = "MachRace|Controls")
	FControlSettings GetControlSettings(float speed);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Theoretical Speed Mult", Keywords = "Get theoretical speed multiplier, used to translate physical speed to theoretical."), Category = "MachRace|Engine")
	float GetTheoreticalSpeedMultiplier() {
		return 0.3176;
	}

	//////////////////////////////////////////////////////////////////////////
	// gameplay related - dynamic settings
	//////////////////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Laser Effective Range", Keywords = "Gets effectrive range of the laser, based on various gameplay conditions."), Category = "MachRace|Gameplay")
	void GetLaserEffectiveRange(float& effectiveRange, float& faloff);

	//////////////////////////////////////////////////////////////////////////
	// gameplay related - speed(s)
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings")
	float Level1BossTriggerSpeed = 2600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings")
	float Level2ObstacleTriggerspeed = 2700;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings")
	float Level2DroneSpawnSpeedLimit = 2800;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings")
	float Level2BossTriggerSpeed = 4500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings")
	float Level3TriggerSpeed = 6000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings")
	float Level3Stage2TriggerSpeed = 12000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings")
	float Level3Stage3TriggerSpeed = 18000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings")
	float Level4TriggerSpeed = 21000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings")
	float Level5TriggerSpeed = 25020;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings|Laser")
	float Level1LaserFalloff = 2500;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings|Laser")
	float Level1LaserEffectiveRange = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings|Laser")
	float Level2LaserFalloff = 4000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings|Laser")
	float Level2LaserEffectiveRange = 5500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings|Laser")
	float Level3LaserFalloff = 5000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings|Laser")
	float Level3LaserEffectiveRange = 7500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings|Laser")
	float DefaultLaserFalloff = 5000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings|Laser")
	float DefaultLaserEffectiveRange = 7500;

	//////////////////////////////////////////////////////////////////////////
	// gameplay related - gameplay global settings
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings")
	float Level1MineDamageAmount = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings")
	float Level1MineHitpoints = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings")
	float Level1ObstacleHitpoints = 15;

	//////////////////////////////////////////////////////////////////////////
	// gameplay related - disables (dev sh1t)
	//////////////////////////////////////////////////////////////////////////

	/** If enabled will prevent level 3 triggering. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool Level3Disable = false;

	/** If enabled will prevent level 4 triggering. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool Level4Disable = false;


	//////////////////////////////////////////////////////////////////////////
	// gameplay related - gameplay events
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnSpawnLevel1Boss OnSpawnLevel1Boss;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnDeathLevel1Boss OnDeathLevel1Boss;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnIntroduceDrone OnIntroduceDrone;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnSpawnEnemy OnSpawnEnemy;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnLevel2Boss OnLevel2Boss;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnLevel3Reached OnLevel3Reached;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnLevel3Boss OnLevel3BossReached;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnLevel4Reached OnLevel4Reached;


	//////////////////////////////////////////////////////////////////////////
	// gameplay related - misc triggers / disables / props
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool Level2GuidanceDroneIntroduced = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool WaterEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool DroneFormationSpawned = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	int32 Level2OnStartTilesToKeepFreeOfObstacles = 3;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool Level2BossDefeated = false;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Level 1 Boss as Defeated", Keywords = "Sets level one boss as defeated, affecting related state conditions."), Category = "MachRace|Gameplay")
	void SetLevelOneBossDeafeated();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Next Speed Trigger", Keywords = "Get next stage speed, such as level switch or boss trigger speed."), Category = "MachRace|Gameplay")
	float GetNextStageSpeed();

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
