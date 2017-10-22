// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QualitySettingManager.generated.h"

UENUM(BlueprintType)
enum class QualitySetting : uint8 {
	Potato				UMETA(DisplayName = "Potato"),
	Toaster 			UMETA(DisplayName = "Toaster"),
	PCMasterRace 		UMETA(DisplayName = "#PCMasterRace"),
	QuantumComputer		UMETA(DisplayName = "Quantum Computer"),
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UQualitySettingManager : public UActorComponent
{
	GENERATED_BODY()
		
private:

	void executeConsoleCommand(FString cmd);

public:	
	// Sets default values for this component's properties
	UQualitySettingManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "MachRace|Video Settings")
	void ApplyPreset(QualitySetting qs);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Debug")
	bool PrintDebug = false;
	
};
