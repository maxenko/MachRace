// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "RaceActorBase.h"
#include "ShipBase.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float ShieldHitpoints;


};
