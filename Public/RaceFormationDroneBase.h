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

	FTimerHandle wobbleTimer;
	FVector wobbleOffset = FVector::ZeroVector;
	void generateRandomOffset();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AssignPosition(USceneComponent* position);
	
	void moveTo(FVector to, float delta, FVector speed);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	USceneComponent* Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector AbandonOffset = FVector(0, 0, 10000);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector FollowSpeed = FVector(10, 10, 10);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector AbandonSpeed = FVector(10, 10, 100);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Abandon Formation", Keywords = "Mark drone as having abandoned the formation."), Category = "MachRace|Gameplay")
	void AbandonFormation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool Wobble = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector2D WobbleRandomRange = FVector2D(.1, .5);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector WobbleOffsetMin = FVector(-2, -2, -2);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	FVector WobbleOffsetMax = FVector(2, 2, 2);
};
