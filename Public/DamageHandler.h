// Copyright 2015 - Max Enko

#pragma once

#include "Components/ActorComponent.h"
#include "CommonTypes.h"
#include "DamageHandler.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComponentDestroyedSignature, USceneComponent*, component);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorDestroyedSignature, AActor*, actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnComponentDamageSignature, USceneComponent*, component, float, health);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorDamageSignature, float, HealthLeft);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHRACE_API UDamageHandler : public UActorComponent {
	GENERATED_BODY()

private:
	FScriptDelegate HandleDamage;

public:	
	// Sets default values for this component's properties
	UDamageHandler();

	UPROPERTY(BlueprintReadOnly, meta=(ClampMin = "0"))
	float Health = 100;

	UPROPERTY(BlueprintReadOnly, meta = (ClampMin = "0"))
	float MaxHealth = 100;

	UPROPERTY(BlueprintReadOnly)
	TArray<FComponentDamageRecord> DamageableComponents;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnComponentDamageSignature OnComponentDamage;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnComponentDestroyedSignature OnComponentDestroyed;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnActorDestroyedSignature OnActorDestroyed;

	UPROPERTY(BlueprintAssignable, Category = "MachRace|Gameplay")
	FOnActorDamageSignature OnActorDamage;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Take Damage", Keywords = "Take Damage and subtract it from Health."), Category = "MachRace|Gameplay")
	float TakeDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, UDamageType* DamageType, AActor* DamageCauser);
	//float TakeDamage(AActor* DamagedActor, float Damage, const FVector& HitFromDirection, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<class UDamageType> DamageTypeClass);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Health", Keywords = "Add an amount to Health."), Category = "MachRace|Gameplay")
	float AddHealth(float amount);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Health", Keywords = "Set Health to specific value."), Category = "MachRace|Gameplay")
	void SetHealth(float amount);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Register Component For Damage", Keywords = "Register component to be tracked for damage."), Category = "MachRace|Gameplay")
	void RegisterComponentForDamage(USceneComponent* c, float initialHealth);

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	virtual void InitializeComponent() override;
};