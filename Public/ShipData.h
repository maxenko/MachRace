// Alpha only, not for distribution.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "CommonTypes.h"

#include "ShipData.generated.h"

/**
 * 
 */
UCLASS()
class MACHRACE_API UShipData : public UDataAsset {
	GENERATED_BODY()
	
//public:

	//////////////////////////////////////////////////////////////////////////
	// Control settings
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, Category = "MachRace|Controls")
	FControlSettings BaseControlSettings;

	UPROPERTY(EditAnywhere, Category = "MachRace|Controls")
	FControlSettings Level1ControlMultipliers;

	UPROPERTY(EditAnywhere, Category = "MachRace|Controls")
	FControlSettings Level1BossControlMultipliers;

	UPROPERTY(EditAnywhere, Category = "MachRace|Controls")
	FControlSettings Level2ControlMultipliers;

	UPROPERTY(EditAnywhere, Category = "MachRace|Controls")
	FControlSettings Level3ControlMultipliers;

	UPROPERTY(EditAnywhere, Category = "MachRace|Controls")
	FControlSettings Level3BossControlMultipliers;

	UPROPERTY(EditAnywhere, Category = "MachRace|Controls")
	FControlSettings Level4ControlMultipliers;

	//////////////////////////////////////////////////////////////////////////
	// Gameplay settings
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, Category = "MachRace|Gameplay")
	float ShieldMaxHitPoints = 500;

	UPROPERTY(EditAnywhere, Category = "MachRace|Gameplay")
	float LaserDamage = 10;

	UPROPERTY(EditAnywhere, Category = "MachRace|Gameplay")
	float LaserDamageMultiplier = 1.0;



	//////////////////////////////////////////////////////////////////////////
	// Animation settings
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, Category = "MachRace|Gameplay")
	float MinDistFromGround = 150;

	UPROPERTY(EditAnywhere, Category = "MachRace|Gameplay")
	float GroundFollowSpeed = 10;

	UPROPERTY(EditAnywhere, Category = "MachRace|Gameplay")
	float MaxBankingYaw = 4.5;

	UPROPERTY(EditAnywhere, Category = "MachRace|Gameplay")
	float MaxBankingYawSpeed = 10;

	UPROPERTY(EditAnywhere, Category = "MachRace|Gameplay")
	float MaxVelocityChangeThreshold = 300;

	UPROPERTY(EditAnywhere, Category = "MachRace|Gameplay")
	float ShipHoverRealignmentSpeed = .6;



};
