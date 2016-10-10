// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Pawn.h"
#include "MenuCameraPawn.generated.h"

UCLASS()
class MACHRACE_API AMenuCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMenuCameraPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	
	
};
