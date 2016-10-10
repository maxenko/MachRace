// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "RaceGameStateBase.h"
#include "CustomExtensions.h"
#include "BasicRaceActor.generated.h"

UCLASS()
class MACHRACE_API ABasicRaceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasicRaceActor();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Ignored Actors", Keywords = "Optional utility collection of actors to ignore, if this actor needs to."), Category = "MachRace|System")
	TArray<AActor*> IgnoredActors;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get MachRace State", Keywords = "Gets MachRace specific GameState."), Category = "MachRace|System")
	ARaceGameStateBase* GetState(bool& success);

};
