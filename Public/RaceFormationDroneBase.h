// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaceFormationDroneBase.generated.h"

UCLASS()
class MACHRACE_API ARaceFormationDroneBase : public ARaceActorBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARaceFormationDroneBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	bool abandoning = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AssignPosition(USceneComponent* position);
	
	void AbandonFormation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	USceneComponent* Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector AbandonOffset = FVector(0, 0, 10000);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector FollowSpeed = FVector(10, 10, 10);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector AbandonSpeed = FVector(10, 10, 100);
};
