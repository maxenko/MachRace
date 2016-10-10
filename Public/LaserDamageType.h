// Copyright 2015 - Max Enko

#pragma once

#include "GameFramework/DamageType.h"
#include "LaserDamageType.generated.h"


UCLASS()
class MACHRACE_API ULaserDamageType : public UDamageType
{
	GENERATED_BODY()
	
public:
	ULaserDamageType() {
		DamageImpulse = 0.f;
		DestructibleImpulse = 0.f;
	}

};
