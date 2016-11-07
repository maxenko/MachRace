// Copyright 2015 - Max Enko

#pragma once

#include "Components/ActorComponent.h"
#include "Hover.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UHover : public UActorComponent
{
	GENERATED_BODY()

private:
	float checkGroundDist(FHitResult& hit);

public:	

	UHover();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	bool DrawDebug = false;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool IsHoverEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool CallHoverManually = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float GroundFollowSpeed = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float MinDistFromGround = 150;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Hover", Keywords = "Hover parent above ground."), Category = "MachRace|System")
	void Hover(float delta);

	virtual void BeginPlay() override;
	
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

};