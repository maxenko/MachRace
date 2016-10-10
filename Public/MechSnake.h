// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "MechSnake.generated.h"

UCLASS()
class MACHRACE_API AMechSnake : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMechSnake();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
