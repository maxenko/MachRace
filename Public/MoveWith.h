// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MoveWith.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UMoveWith : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMoveWith();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	AActor* Target = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector Offset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	bool EnableFollow = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	FVector FollowSpeed = FVector(1,1,1);

};
