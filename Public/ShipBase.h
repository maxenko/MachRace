// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "RaceActorBase.h"
#include "ShipBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldCharge);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldDeplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldDepleted);

UCLASS()
class MACHRACE_API AShipBase : public ARaceActorBase {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShipBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Ship Speed", Keywords = "Sets speed ship to a specific (theoretical) value."), Category = "MachRace|Utility")
	void SetShipSpeed(float speed);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float ShieldHitPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float ShieldMaxHitPoints;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Charge Shield", Keywords = "Recharge shield hitpoints."), Category = "MachRace|Gameplay")
	void ChargeShield(float amount);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Deplete Shield", Keywords = "Deplete shield hitpoints."), Category = "MachRace|Gameplay")
	void DepleteShield(float amount);

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnShieldCharge OnShieldCharge;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnShieldDeplete OnShieldDeplete;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnShieldDepleted OnShieldDepleted;
};
