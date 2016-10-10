// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "Engine/LevelScriptActor.h"
#include "RaceLevelBase.generated.h"

UCLASS()
class MACHRACE_API ARaceLevelBase : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARaceLevelBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;


	
};
