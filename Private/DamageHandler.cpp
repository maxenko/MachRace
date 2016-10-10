// Copyright 2015 - Max Enko

#include "MachRace.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DamageHandler.h"

// Sets default values for this component's properties
UDamageHandler::UDamageHandler() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

float UDamageHandler::TakeDamage(AActor* DamagedActor, float Damage, const FVector& HitFromDirection, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<class UDamageType> DamageTypeClass) {

	// if there are components present, take damage for component being hit
 	if (DamageableComponents.Num() > 0) {

		for (FComponentDamageRecord& r : DamageableComponents) {

			// pass on previously destroyed components
			if (!UKismetSystemLibrary::IsValid(r.Component)) {
				continue;
			}

			if (r.Component == HitInfo.GetComponent()) {
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
			OnActorDestroyed.Broadcast(GetOwner());
		}
		return Health;
	}
}

float UDamageHandler::AddHealth(float amount) {
	if (MaxHealth <= 0) {
		Health = FMath::Clamp(Health+amount,0.f, 999999.f);
	} else {
		Health = FMath::Clamp(amount, 0.0f, MaxHealth);
	}
	return Health;
}

void UDamageHandler::SetHealth(float amount){
	if (MaxHealth <= 0) {
		Health = FMath::Clamp(amount, 0.f, 999999.f);
	} else {
		Health = FMath::Clamp(amount, 0.0f, MaxHealth);
	}
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

	// set up delegates
	HandleDamage.BindUFunction(this, TEXT("TakeDamage"));

	AActor* a = GetOwner();
	//a->OnTakePointDamage.Add(HandleDamage);
}


void UDamageHandler::RegisterComponentForDamage(USceneComponent* c, float initialHealth) {
	FComponentDamageRecord r;
	r.Component = c;
	r.Health = initialHealth;
	DamageableComponents.Add(r);
}