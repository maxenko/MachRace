// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Pawn.h"
#include "PawnBase.generated.h"

UCLASS()
class MACHRACE_API APawnBase : public APawn
{
	GENERATED_BODY()

private:
	

public:

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Camera", Keywords = "Update MachRace camera with gameplay specific settings."), Category = "Presentation|Camera")
	void UpdateCamera(UCameraComponent* c, UStaticMeshComponent* hud, FTransform t, float fov, float hudScaleMultiplier, float speed);

	// Sets default values for this pawn's properties
	APawnBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

};
