// Copyright 2015 - Max Enko

#pragma once

#include "Components/ActorComponent.h"
#include "Autopilot.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UAutopilot : public UActorComponent
{
	GENERATED_BODY()

private:

	
	void keepUpWithTarget();

public:	
	// Sets default values for this component's properties
	UAutopilot();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "FollowTarget", Keywords = "Follow target, accelerating and decelerating as necessary."), Category = "MachRace|Gameplay")
	void AdjustVelocityToFollowTarget(float delta);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float NavigationSpeed = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool FollowTarget = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float FllowDistance = -1000;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System")
	FVector Velocity = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	AActor* Target;
	
};
