// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaceActorBase.h"
#include "ProjectileBase.h"
#include "PulseLaserBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPulseFire, FVector, Velocity);

UCLASS()
class MACHRACE_API APulseLaserBase : public ARaceActorBase {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APulseLaserBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	UPrimitiveComponent* OwnerPhysicsComponent = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	AActor* Target = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	USceneComponent* EmitOrigin = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	TSubclassOf<AProjectileBase> ProjectileTemplate;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "MachRace|System")
	FOnPulseFire OnFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float ProjectileSpeed = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	float RateOfFireMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Gameplay")
	bool EnableFire = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|Presentation")
	USoundCue* FireSound = NULL;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Fire Direction", Keywords = "Gets directional vector (normilized) from the EmnitOrigin to the Target."), Category = "MachRace|System")
	FVector GetDirectionToTarget() {

		if (!Target) {
			return FVector::ZeroVector;
		}

		if (!EmitOrigin) {
			return FVector::ZeroVector;
		}

		auto dir = (Target->GetActorLocation() - EmitOrigin->GetComponentLocation());
		dir.Normalize();

		return dir;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Projectile Velocity", Keywords = "Calculates velocity of the projectile toward the Target."), Category = "MachRace|System")
	FVector GetProjectileVelocity() {

		if (!OwnerPhysicsComponent) {
			return FVector::ZeroVector;
		}

		return (ProjectileSpeed * GetDirectionToTarget()) + OwnerPhysicsComponent->GetPhysicsLinearVelocity();
	}
};
