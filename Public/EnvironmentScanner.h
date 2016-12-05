// Copyright 2015 - Max Enko

#pragma once

#include "Components/ActorComponent.h"
#include "EnvironmentScanner.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UEnvironmentScanner : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnvironmentScanner();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY(BlueprintReadOnly, Category = "MachRace|System")
	TArray<FHitResult> LastScan;

	UFUNCTION(BlueprintCallable, Category = "MachRace|System")
	virtual TArray<FHitResult> Scan();
};
