#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "DamageInterface.generated.h"


class ABaseWeapon;
class ABaseCharacter;

UCLASS(Abstract)
class UDamageInterface : public UInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintPure)
	static float CalculateDamage(FActiveGameplayEffectHandle Handle, FGameplayEffectSpec& Spec,
		FGameplayEffectCustomExecutionParameters& ExecutionParams);
};
