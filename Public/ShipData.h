// Alpha only, not for distribution.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "CommonTypes.h"

#include "ShipData.generated.h"

UCLASS(BlueprintType)
class MACHRACE_API UShipData : public UPrimaryDataAsset {
	GENERATED_BODY()

public:

	//////////////////////////////////////////////////////////////////////////
	// Identification settings
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identification")
	FString ShipId;

	UPROPERTY(EditAnywhere, Category = "Identification")
	FString ShipDislayName;


	//////////////////////////////////////////////////////////////////////////
	// Control settings
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	FControlSettings BaseControlSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	FControlSettings Level1ControlMultipliers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	FControlSettings Level1BossControlMultipliers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	FControlSettings Level2ControlMultipliers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	FControlSettings Level2BossControlMultipliers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	FControlSettings Level3ControlMultipliers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	FControlSettings Level3BossControlMultipliers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	FControlSettings Level4ControlMultipliers;


	//////////////////////////////////////////////////////////////////////////
	// Gameplay settings
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float ShieldMaxHitPoints = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float ShieldChargeMultiplier = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float LaserDamage = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float LaserDamageMultiplier = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float AccelerationMultiplier = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float BoostMultiplier = 1.0;


	//////////////////////////////////////////////////////////////////////////
	// Animation settings
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MinDistFromGroundPrelaunch = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MinDistFromGroundLevel1 = 135;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MinDistFromGroundLevel1Boss = 135;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MinDistFromGroundLevel2 = 150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MinDistFromGroundLevel2Boss = 150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MinDistFromGroundLevel3 = 150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MinDistFromGroundLevel3Boss = 150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MinDistFromGroundLevel4 = 150;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float GroundFollowSpeedPrelaunch = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float GroundFollowSpeedLevel1 = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float GroundFollowSpeedLevel1Boss = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float GroundFollowSpeedLevel2 = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float GroundFollowSpeedLevel2Boss = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float GroundFollowSpeedLevel3 = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float GroundFollowSpeedLevel3Boss = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float GroundFollowSpeedLevel4 = 50;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MaxBankingYaw = 4.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MaxBankingYawSpeed = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MaxVelocityChangeThreshold = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float ShipHoverRealignmentSpeed = .6;



	//////////////////////////////////////////////////////////////////////////
	// Appearance settings
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	ARaceShipBase* GameplayShipModel = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	AActor* DisplayShipModel = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FColor LaserColor = FColor::Blue;


	//////////////////////////////////////////////////////////////////////////
	// Convenience Methods
	//////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetLeapImpulse", Keywords = "Get Leap Impulse based on a level."), Category = "MachRace|Gameplay")
	float GetLeapImpulse(GameStage stage) {

		switch (stage){
			case GameStage::Desert:
				return BaseControlSettings.LeapImpulse * Level1ControlMultipliers.LeapImpulse;

			case GameStage::DesertBoss:
				return BaseControlSettings.LeapImpulse * Level1BossControlMultipliers.LeapImpulse;

			case GameStage::InfiniteHex:
				return BaseControlSettings.LeapImpulse * Level2ControlMultipliers.LeapImpulse;

			case GameStage::InfiniteHexBoss:
				return BaseControlSettings.LeapImpulse * Level2BossControlMultipliers.LeapImpulse;

			case GameStage::Labyrinth:
				return BaseControlSettings.LeapImpulse * Level3ControlMultipliers.LeapImpulse;

			case GameStage::LabyrinthBoss:
				return BaseControlSettings.LeapImpulse * Level3BossControlMultipliers.LeapImpulse;

			case GameStage::Space:
				return BaseControlSettings.LeapImpulse * Level4ControlMultipliers.LeapImpulse;

			default:
				return BaseControlSettings.LeapImpulse;
		}
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetBankingSpeedImpulse", Keywords = "Get Banking Speed Impulse based on a level."), Category = "MachRace|Gameplay")
	float GetBankingSpeedImpulse(GameStage stage) {

		switch (stage) {
			case GameStage::Desert:
				return BaseControlSettings.BankingSpeedImpulse * Level1ControlMultipliers.BankingSpeedImpulse;

			case GameStage::DesertBoss:
				return BaseControlSettings.BankingSpeedImpulse * Level1BossControlMultipliers.BankingSpeedImpulse;

			case GameStage::InfiniteHex:
				return BaseControlSettings.BankingSpeedImpulse * Level2ControlMultipliers.BankingSpeedImpulse;

			case GameStage::InfiniteHexBoss:
				return BaseControlSettings.BankingSpeedImpulse * Level2BossControlMultipliers.BankingSpeedImpulse;

			case GameStage::Labyrinth:
				return BaseControlSettings.BankingSpeedImpulse * Level3ControlMultipliers.BankingSpeedImpulse;

			case GameStage::LabyrinthBoss:
				return BaseControlSettings.BankingSpeedImpulse * Level3BossControlMultipliers.BankingSpeedImpulse;

			case GameStage::Space:
				return BaseControlSettings.BankingSpeedImpulse * Level4ControlMultipliers.BankingSpeedImpulse;

			default:
				return BaseControlSettings.BankingSpeedImpulse;
		}
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetBankingRotationImpulse", Keywords = "Get Banking Rotation Impulse based on a level."), Category = "MachRace|Gameplay")
	float GetBankingRotationImpulse(GameStage stage) {

		switch (stage) {
			case GameStage::Desert:
				return BaseControlSettings.BankingRotationImpulse * Level1ControlMultipliers.BankingRotationImpulse;

			case GameStage::DesertBoss:
				return BaseControlSettings.BankingRotationImpulse * Level1BossControlMultipliers.BankingRotationImpulse;

			case GameStage::InfiniteHex:
				return BaseControlSettings.BankingRotationImpulse * Level2ControlMultipliers.BankingRotationImpulse;

			case GameStage::InfiniteHexBoss:
				return BaseControlSettings.BankingRotationImpulse * Level2BossControlMultipliers.BankingRotationImpulse;

			case GameStage::Labyrinth:
				return BaseControlSettings.BankingRotationImpulse * Level3ControlMultipliers.BankingRotationImpulse;

			case GameStage::LabyrinthBoss:
				return BaseControlSettings.BankingRotationImpulse * Level3BossControlMultipliers.BankingRotationImpulse;

			case GameStage::Space:
				return BaseControlSettings.BankingRotationImpulse * Level4ControlMultipliers.BankingRotationImpulse;

			default:
				return BaseControlSettings.BankingRotationImpulse;
		}
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetAccelerationAmount", Keywords = "Get Acceleration Amount."), Category = "MachRace|Gameplay")
	float GetAccelerationAmount(float baseAmount) {
		return AccelerationMultiplier * baseAmount;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetShieldChargeAmount", Keywords = "Get Shield Charge Amount."), Category = "MachRace|Gameplay")
	float GetShieldChargeAmount(float baseAmount) {
		return ShieldChargeMultiplier * baseAmount;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetBoostAmount", Keywords = "Get Boost Amount."), Category = "MachRace|Gameplay")
	float GetBoostAmount(float baseAmount) {
		return BoostMultiplier * baseAmount;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetLaserDamageAmount", Keywords = "Get Laser Damage Amount."), Category = "MachRace|Gameplay")
	float GetLaserDamageAmount(float baseAmount) {
		return LaserDamageMultiplier * baseAmount;
	}
};