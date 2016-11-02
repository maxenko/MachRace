// Copyright 2015 - Max Enko

#pragma once

#include "RaceActorBase.h"
#include "RaceDroneBase.generated.h"

/**
 * 
 */
UCLASS()
class MACHRACE_API ARaceDroneBase : public ARaceActorBase
{
	GENERATED_BODY()
		
private:



public:



	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
};
