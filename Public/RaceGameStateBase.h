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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRemoveLevel2Boss);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevel3Reached);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevel3Boss);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevel4Reached);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnEnemy, GameStage, stage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStageChange, GameStage, previousStage, GameStage, newStage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCCAdded, int32, amount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAutoAimTargetAcquired, AActor*, target);

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
	bool level2BossSetToBeRemoved = false;

	// delta time history
	TArray<float> deltaTimeHistory;
	int32 fpsIndexCounter = 0;
	
public:

	ARaceGameStateBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	//////////////////////////////////////////////////////////////////////////
	// machrace system related
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(BlueprintReadWrite, Category = "MachRace|System")
	int32 DeltaTimeHistorySize = 10;
	
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

	AActor* LastAutoAimTarget = NULL;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System|Gameplay")
	ARaceActorBase* CurrentAttacker = NULL;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Request Attacker Status", Keywords = "Request to become primary attacker."), Category = "MachRace|System")
	bool RequestAttackerStatus(ARaceActorBase* requester);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Release Attacker Status", Keywords = "Release self from primary attacker status."), Category = "MachRace|System")
	bool ReleaseAttackerStatus(ARaceActorBase* attacker);

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

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Reset Desert Tile Count", Keywords = "Resets tile count to given number. Defaults to 0."), Category = "MachRace|Controls")
	void ResetDesertTileCount(int to = 0) {
		Level1Index = 0;
	}

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


	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Theoretical Speed Mult", Keywords = "Get theoretical speed multiplier, used to translate physical speed to theoretical."), Category = "MachRace|Engine")
	float GetTheoreticalSpeedMultiplier() {
		return 0.3176;
	}

	//////////////////////////////////////////////////////////////////////////
	// gameplay related - dynamic settings (things that chance based on game state)
	//////////////////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Laser Effective Range", Keywords = "Gets effectrive range of the laser, based on various gameplay conditions."), Category = "MachRace|Gameplay")
	void GetLaserEffectiveRange(float& effectiveRange, float& faloff);


	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Control settings", Keywords = "Get controls settings based on game stage and speed."), Category = "MachRace|Controls")
	FControlSettings GetControlSettings(float speed);


	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Level3 Obstacle Count", Keywords = "Gets Level 3 Stage 1 obstacle count."), Category = "MachRace|Level3|Gameplay")
	int32 GetLevel3Stage1ObstacleCount();


	//////////////////////////////////////////////////////////////////////////
	// gameplay related - speed(s)
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level1")
	float Level1BossTriggerSpeed = 2600;

	/////////////////////// level 2 //////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2")
	float Level2ObstacleTriggerspeed = 2700;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2")
	float Level2ObstacleTriggerspeedThreshold1 = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2")
	float Level2ObstacleTriggerspeedThreshold2 = 3600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2")
	float Level2ObstacleTriggerspeedThreshold3 = 4400;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2")
	float Level2ObstacleTriggerspeedThreshold4 = 4700;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2")
	float Level2ObstacleTriggerspeedThreshold5 = 5200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2")
	float Level2DroneSpawnSpeedLimit = 2800;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2")
	float Level2BossTriggerSpeed = 6000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2")
	float Level2DroneProjectileExplosionRadiusY = 400;

	/////////////////////// level 2 - obstacle weight settings ///////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2|ObstacleWeights", Meta = (DisplayName = "Less Than Trigger Speed"))
	FHexTileDistribuition L2ObstacleWeightsLessThanTriggerSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2|ObstacleWeights", Meta = (DisplayName = "Less Than Threshold 1"))
	FHexTileDistribuition L2ObstacleWeightsLessThanThreshold1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2|ObstacleWeights", Meta = (DisplayName = "Less Than Threshold 2"))
	FHexTileDistribuition L2ObstacleWeightsLessThanThreshold2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2|ObstacleWeights", Meta = (DisplayName = "Less Than Threshold 3"))
	FHexTileDistribuition L2ObstacleWeightsLessThanThreshold3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2|ObstacleWeights", Meta = (DisplayName = "Less Than Threshold 4"))
	FHexTileDistribuition L2ObstacleWeightsLessThanThreshold4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2|ObstacleWeights", Meta = (DisplayName = "Less Than Threshold 5"))
	FHexTileDistribuition L2ObstacleWeightsLessThanThreshold5;

	/////////////////////// level 3 //////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level3")
	float Level3TriggerSpeed = 12000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level3")
	float Level3Stage2TriggerSpeed = 18000;

	/////////////////////// level 3 //////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level4")
	float Level4TriggerSpeed = 25038.72;


	//////////////////////////////////////////////////////////////////////////
	// laser gameplay related - speed(s)
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings|Laser")
	float Level1LaserFalloff = 2500;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings|Laser")
	float Level1LaserEffectiveRange = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings|Laser")
	float Level2LaserFalloff = 4000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings|Laser")
	float Level2LaserEffectiveRange = 5500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings|Laser")
	float Level2BossLaserFalloff = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Settings|Laser")
	float Level2BossLaserEffectiveRange = 20000;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level3")
	bool Level3Disable = false;

	/** If enabled will prevent level 4 triggering. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level4")
	bool Level4Disable = false;


	//////////////////////////////////////////////////////////////////////////
	// gameplay related - gameplay events
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnStageChange OnOnStageChange;

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

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FOnRemoveLevel2Boss OnRemoveLevel2Boss;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnLevel3Reached OnLevel3Reached;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnLevel3Boss OnLevel3BossReached;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnLevel4Reached OnLevel4Reached;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnCCAdded OnCCAdded;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnAutoAimTargetAcquired OnAutoAimTargetAcquired;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get LastAutoAimTarget Safe", Keywords = "Gets last autoaim target, if one exists and is still valid."), Category = "MachRace|Engine")
	AActor* GetAutoAimTargetSafe(bool& success);

	//////////////////////////////////////////////////////////////////////////
	// gameplay related - misc triggers / disables / props
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool DroneFormationSpawned = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2")
	bool Level2BossEnableFiring = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2")
	int32 Level2OnStartTilesToKeepFreeOfObstacles = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool IsInGame = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool ControlsEnabled = false;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	float TopSpeed = 0;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|Gameplay")
	float TopXVelocity = 0;

	UPROPERTY(BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool DisableObstacles = false;

	UPROPERTY(BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool DisableDecorativeGeometry = false;

	UPROPERTY(BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool DisableAccelerators = false;

	UPROPERTY(BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool EnableAutoAim = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level1")
	bool Level1BossDefeated = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level2")
	bool Level2BossDefeated = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay|Level3")
	bool SafeToTransitionToLevel3 = false;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Level 1 Boss as Defeated", Keywords = "Sets level one boss as defeated, affecting related state conditions."), Category = "MachRace|Gameplay")
	void SetLevelOneBossDeafeated();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Next Speed Trigger", Keywords = "Get next stage speed, such as level switch or boss trigger speed."), Category = "MachRace|Gameplay")
	float GetNextStageSpeed();

	//////////////////////////////////////////////////////////////////////////
	// gameplay related - finance / cc
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(BlueprintReadWrite, Category = "MachRace|Gameplay|CC")
	int32 CurrentSessionCC = 0;

	UPROPERTY(BlueprintReadWrite, Category = "MachRace|Gameplay|CC")
	int32 TotalCC = 0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "InRaceCollectCC", Keywords = "Adds CC as collected during normal gameplay."), Category = "MachRace|Gameplay")
	bool InRaceCollectCC(int32 amount);

	//////////////////////////////////////////////////////////////////////////
	// presentation 
	//////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Camera Settings", Keywords = "Get camera settings based on ship speed and stage of the game."), Category = "MachRace|Presentation")
	FCameraSettings GetCameraSettings(float speed);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Mach Atmosphere Settings", Keywords = "Get atmosphere settings based on stage of the game."), Category = "MachRace|Presentation")
	void UpdateAtmosphereSettings();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get UnderFade Settings", Keywords = "Get ship underfade settings."), Category = "MachRace|Presentation")
	FUnderfadeSettings GetUnderFadeSettings();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level3")
	float Level3SonicBoomMaxOpacity = 0.05;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Sonic Boom Opacity", Keywords = "Get maxium opacity for sonic boom effect. Takes current game state into account."), Category = "MachRace|Presentation")
	float GetSonicBoomEffectOpacity();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Exponential Fog", Keywords = "Get reference to content Exponential Fog component."), Category = "MachRace|Presentation")
	AExponentialHeightFog* GetExponentialFog(bool& success);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	AExponentialHeightFog* ExponentialFog;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level2")
	FVector2D Level2RaisedTowerRange = FVector2D(100,500);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level4")
	FTransform Level4PlanetTransform = FTransform::Identity;


	//////////////////////////////////////////////////////////////////////////
	// presentation - camera state
	//////////////////////////////////////////////////////////////////////////


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level1|Stage1|CameraRotation")
	FRotator Level1Stage1CameraRotation = FRotator(-29, -180, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level1|Stage2|CameraRotation")
	FRotator Level1Stage2CameraRotation = FRotator(-29, -180, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level1|Stage1|CameraPosition")
	FVector Level1Stage1CameraPosition = FVector(275, 0, 135);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level1|Stage2|CameraPosition")
	FVector Level1Stage2CameraPosition = FVector(230, 0, 150);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level2|Stage2|CameraPosition")
	FVector Level2Stage1CameraPosition = FVector(245, 0, 320);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level2|Stage2|CameraRotation")
	FRotator Level2Stage1CameraRotation = FRotator(-29, -180, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level2|Stage2|CameraPosition")
	FVector Level2Stage2CameraPosition = FVector(300, 0, 460);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level2|Stage2|CameraRotation")
	FRotator Level2Stage2CameraRotation = FRotator(-32, -180, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level2|Stage2|CameraPosition")
	FVector Level2BossCameraPosition = FVector(300, 0, 460);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level2|Stage2|CameraRotation")
	FRotator Level2BossCameraRotation = FRotator(-32, -180, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level3|Stage1|CameraPosition")
	FVector Level3Stage1CameraPosition = FVector(230, 0, 950);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level3|Stage1|CameraRotation")
	FRotator Level3Stage1CameraRotation = FRotator(-50, -180, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level3|Stage2|CameraPosition")
	FVector Level3Stage2CameraPosition = FVector(-500, 0, 1200);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level3|Stage2|CameraRotation")
	FRotator Level3Stage2CameraRotation = FRotator(-90, -180, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level3|Stage2|CameraFOV")
	float Level3Stage2CameraFOV = 120.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level4|CameraPosition")
	FVector Level4CameraPosition = FVector(230, 0, 950);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level4|CameraRotation")
	FRotator Level4CameraRotation = FRotator(-50, -180, 0);


	//////////////////////////////////////////////////////////////////////////
	// presentation - post process effects
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level1")
	float BoostBlurAmountLevel1 = 4.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation|Level1")
	float BoostBlurAmountLevel2 = 2.5;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetStageBoostBlurAmount", Keywords = "Gets amount of boost blue appropriate for this stage."), Category = "MachRace|Gameplay")
	float GetStageBoostBlurAmount();

	//////////////////////////////////////////////////////////////////////////
	// statistics
	//////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Average FPS", Keywords = "Claculates average of internal DeltaTime history, returns average FPS (not deltatime)."), Category = "MachRace|Statistics")
	float GetAverageFps();

};
