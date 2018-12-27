#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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


	public:

	AMenuV4PlayerControllerBase();
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MachRace|Input")
	FOnMouseMove OnMouseMove;
	
};
