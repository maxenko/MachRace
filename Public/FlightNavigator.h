// Copyright 2015 - Max Enko

#pragma once

#include "Components/ActorComponent.h"
#include "FlightNavigator.generated.h"



USTRUCT(BlueprintType)
struct FFlightNavigationRay {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	FVector From = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	FVector To = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	float Weight = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	float Distance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Engine")
	FHitResult Hit;
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UFlightNavigator : public UActorComponent
{
	GENERATED_BODY()

private:

	// generate scan field intervals
	TArray<float> getIntervals();
	TArray<FFlightNavigationRay> getScan();

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector FollowOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool DodgeObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	int32 DetectionRays = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float DetectionRayInterval = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float ScanDistance = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool DrawDebug = true;

	UFUNCTION(BlueprintCallable, Category = "MachRace|Gameplay")
	TArray<FVector> DetectObstacles();

	// Sets default values for this component's properties
	UFlightNavigator();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

};
