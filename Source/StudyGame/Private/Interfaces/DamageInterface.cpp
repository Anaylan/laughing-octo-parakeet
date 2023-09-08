
#include "Interfaces/DamageInterface.h"
#include "Character/BaseCharacter.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayEffect.h"

float UDamageInterface::CalculateDamage(FActiveGameplayEffectHandle Handle, FGameplayEffectSpec& Spec,
		FGameplayEffectCustomExecutionParameters& ExecutionParams)
{
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!TargetASC)
	{
		return 0.f;
	}

	FGameplayEffectSpec Specs = ExecutionParams.GetOwningSpec();

	// Specs.TargetEffectSpecs.Targ
	
	ABaseCharacter* SourceActor = static_cast<ABaseCharacter*>(Spec.GetEffectContext().GetEffectCauser());
	// AActor* TargetActor = static_cast<AActor*>(Spec);
	if (!SourceActor) return 0.f;
	return SourceActor->GetDamage();
}
