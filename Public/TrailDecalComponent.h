// Copyright 2015 - Max Enko

#pragma once

#include "Components/SceneComponent.h"
#include "TrailDecalComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UTrailDecalComponent : public USceneComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UTrailDecalComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

		
	
};
