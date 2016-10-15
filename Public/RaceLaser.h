// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "RaceActorBase.h"
#include "RaceLaser.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFireDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndFireDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitDelegate, FHitResult, hit);


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
	FHitDelegate HasHit;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FFireDelegate HitEnded;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FFireDelegate NoHit;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FFireDelegate StartFiring;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Gameplay")
	FFireDelegate EndFiring;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	TArray<TEnumAsByte<EObjectTypeQuery>> AutoAimQueryParams;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

};
