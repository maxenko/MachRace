// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "PawnBase.h"
#include "RaceShipBase.h"
#include "RaceGameStateBase.h"
#include "RacePlayerBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTethered, ARaceShipBase*, Ship);

UCLASS()
class MACHRACE_API ARacePlayerBase : public APawnBase
{
	GENERATED_BODY()

private:
	void trackShip();
	TArray<FVector> followHistory;
	void recordFloorTraceToHistory();
	
public:	
	// Sets default values for this actor's properties
	ARacePlayerBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Camera", Keywords = "Update MachRace camera with gameplay specific settings."), Category = "Presentation|Camera")
	void UpdateCamera(UCameraComponent* c, UStaticMeshComponent* hud, FTransform t, float fov, float hudScaleMultiplier, float speed);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	ARaceShipBase* RaceShip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool IsTethered = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	int32 FollowHistorySize;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Content Instancer", Keywords = "Get reference to content instancer."), Category = "Utility|Engine")
	ARaceShipBase* GetRaceShip(bool& success);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Tether Player to Ship", Keywords = "Sets the player to follow its RaceShip."), Category = "Utility|Engine")
	bool TetherTo(ARaceShipBase* ship);

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnTethered OnThethered;
};
