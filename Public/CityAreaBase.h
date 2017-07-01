// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/Actor.h"
#include "RaceActorBase.h"
#include "CityAreaBase.generated.h"

UCLASS()
class MACHRACE_API ACityAreaBase : public ARaceActorBase
{
	GENERATED_BODY()

			
public:	
	// Sets default values for this actor's properties
	ACityAreaBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Generate Grid", Keywords = "Generate city grid based on half settings and span of eachg cell."), Category = "MachRace|System")
	void GenerateGrid();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	int32 HalfGridX = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	int32 HalfGridY = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float Spacing = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	TArray<FVector> IntersectionsLocations;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	AActor* Target = NULL;


};
