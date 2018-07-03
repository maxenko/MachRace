// Copyright 2015 - Max Enko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

UCLASS()
class MACHRACE_API AProjectileBase : public ARaceActorBase {
	GENERATED_BODY()



	public:
	// Sets default values for this actor's properties
	AProjectileBase();

	protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	private:
	
	FDateTime lifeBeganTime;

	public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Fire Projectile", Keywords = "Sets the volocity to the projectile, firing it. Returns false if physics component of the projectile could not be found."), Category = "MachRace|System")
	bool SetProjectileVelocity(FVector velocity);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MachRace|System")
	float MaxLifeTime = 5.0f;

};
