// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "RaceActorBase.h"
#include "RaceLaser.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFireDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndFireDelegate);

UCLASS()
class MACHRACE_API ARaceLaser : public ARaceActorBase
{
	GENERATED_BODY()

private:

	bool traceAhead();
	bool isHitting = false;
	bool previousIsFiring = false;
	
public:	
	// Sets default values for this actor's properties
	ARaceLaser();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsFiring;

	UPROPERTY(BlueprintReadOnly)
	FVector DefaultDirection = FVector(-1, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Direction = FVector(-50000,0,0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector From = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector To = From + Direction;

	UPROPERTY(BlueprintReadOnly)
	FHitResult LastHit;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FFireDelegate Fire;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FEndFireDelegate EndFire;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Laser Hit"), Category = "MachRace|Gameplay")
	void OnLaserHit(FHitResult hit);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Laser End Hit"), Category = "MachRace|Gameplay")
	void OnLaserEndHit();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Laser No Hit"), Category = "MachRace|Gameplay")
	void OnLaserNoHit();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Laser Start Firing"), Category = "MachRace|Gameplay")
	void OnLaserStartFiring();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Laser End Firing"), Category = "MachRace|Gameplay")
	void OnLaserEndFiring();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

};
