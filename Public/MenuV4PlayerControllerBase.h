#pragma once

#include "CoreMinimal.h"
#include "CommonTypes.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "MenuV4PlayerControllerBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMouseMove, float, X, float, Y);

/**
 * Handles various menu routines.
 */
UCLASS()
class MACHRACE_API AMenuV4PlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
	
	private:

	float mouseX = 0.0;
	float mouseY = 0.0;

	float previousMouseX = 0.0;
	float previousMouseY = 0.0;


	protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	public:

	AMenuV4PlayerControllerBase();
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetInputActionSettingFor", Keywords = "Gets input action setting for specific action and input type. One settings per input."), Category = "MachRace|Input")
	FInputActionKeyMapping GetInputActionSettingFor(bool& success, FName name, MachRaceInputType inputType);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Input")
	FOnMouseMove OnMouseMove;
};