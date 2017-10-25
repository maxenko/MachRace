// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResolutionSettingManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UResolutionSettingManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UResolutionSettingManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Supported Resolutions", Keywords = "Gets a list of supported resolutions."), Category = "Machrace|Settings")
	TArray<FString> GetSupportedResolutions();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Config Resolution", Keywords = "Gets resolution from config, if there is one."), Category = "Machrace|Settings")
	FString GetConfigResolution(bool& success);
};
