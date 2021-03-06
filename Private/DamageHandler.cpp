// Copyright 2015 - Max Enko

#include "DamageHandler.h"
#include "MachRace.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
UDamageHandler::UDamageHandler() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

float UDamageHandler::TakeDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, UDamageType* DamageType, AActor* DamageCauser) {

	// if there are components present, take damage for component being hit
 	if (DamageableComponents.Num() > 0) {

		for (FComponentDamageRecord& r : DamageableComponents) {

			// pass on previously destroyed components
			if (!UKismetSystemLibrary::IsValid(r.Component)) {
				continue;
			}

			if (r.Component == FHitComponent) {
				// set health
				r.Health = FMath::Clamp(r.Health - Damage, 0.f, 999999.f);
				OnComponentDamage.Broadcast(r.Component, r.Health);

				// if at zero, call component destroyed event
				if (r.Health <= 0) {
					OnComponentDestroyed.Broadcast(r.Component);
				}
				return r.Health;
			}
		}
		return Health; // return actor health, this should never happen

	// otherwise take damage as actor
	} else {

 		Health = FMath::Clamp(Health - Damage, 0.f, Health);

		// if at zero, call actor destroyed event
		if (Health <= 0) {
			Health = 0;
			auto o = GetOwner();
			if (o) {
				OnActorDestroyed.Broadcast(o, HitLocation);
			}
			
		} else {
			OnActorDamage.Broadcast(Health);
		}
		return Health;
	}
}

float UDamageHandler::AddHealth(float amount) {
	if (MaxHealth <= 0) {
		Health = FMath::Clamp(Health+amount,0.f, 999999.f);
	} else {
		Health = FMath::Clamp(Health+amount, 0.0f, MaxHealth);
	}
	return Health;
}

void UDamageHandler::SetHealth(float amount){
	Health = FMath::Clamp(amount, 0.f, 999999.f);
}

void UDamageHandler::SetMaxHealth(float amount) {
	MaxHealth = FMath::Clamp(amount, 0.f, 999999.f);
}

// Called when the game starts
void UDamageHandler::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void UDamageHandler::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

void UDamageHandler::InitializeComponent() {

	// set up delegate for taking damage on the owner actor
	HandleDamage.BindUFunction(this, TEXT("TakeDamage"));

	AActor* a = GetOwner();
	a->OnTakePointDamage.Add(HandleDamage);
}

void UDamageHandler::RegisterComponentForDamage(USceneComponent* c, float initialHealth) {
	FComponentDamageRecord r;
	r.Component = c;
	r.Health = initialHealth;
	DamageableComponents.Add(r);
}