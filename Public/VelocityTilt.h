// Copyright 2015 - Max Enko

#pragma once

#include "Components/ActorComponent.h"
#include "VelocityTilt.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UVelocityTilt : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVelocityTilt();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	UPrimitiveComponent* TiltComponent;
	
};
