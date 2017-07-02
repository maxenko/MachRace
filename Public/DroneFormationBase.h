// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DroneFormationBase.generated.h"

UCLASS()
class MACHRACE_API ADroneFormationBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADroneFormationBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool IsAutoAimTarget = false;
	
};
