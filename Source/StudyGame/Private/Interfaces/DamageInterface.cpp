
#include "Interfaces/DamageInterface.h"
#include "Character/BaseCharacter.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayEffect.h"

void UDamageInterface::ApplyDamage(FActiveGameplayEffectHandle Handle, FGameplayEffectSpec& Spec,
		FGameplayEffectCustomExecutionParameters& ExecutionParams)
{
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!TargetASC)
	{
		return;
	}

	FGameplayEffectSpec Specs = ExecutionParams.GetOwningSpec();

	// Specs.TargetEffectSpecs.Targ
	
	AActor* SourceActor = static_cast<AActor*>(Spec.GetEffectContext().GetSourceObject());
	// AActor* TargetActor = static_cast<AActor*>(Spec);
	if (!SourceActor) return;

	
}
