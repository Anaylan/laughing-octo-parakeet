// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Attributes/BaseAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Character/BaseCharacter.h"
#include "Net/UnrealNetwork.h"

void UBaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseAttributeSet, Health)
	DOREPLIFETIME(UBaseAttributeSet, MaxHealth)
	DOREPLIFETIME(UBaseAttributeSet, Damage)
	DOREPLIFETIME(UBaseAttributeSet, SpeedMultiplier)
}

void UBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	FGameplayEffectContextHandle ContextHandle = Data.EffectSpec.GetContext();

	UAbilitySystemComponent* Source = ContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	float Delta = 0.f;

	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Additive)
	{
		Delta = Data.EvaluatedData.Magnitude;
	}

	ABaseCharacter* TargetCharacter = nullptr;

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		AActor* TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();

		TargetCharacter = StaticCast<ABaseCharacter*>(TargetActor);
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		UE_LOG(LogTemp, Warning, TEXT("DMG attribute"))
		AActor* SourceActor = nullptr;
		ABaseCharacter* SourceCharacter = nullptr;

		if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
		{
			AController* SourceController = nullptr;
			SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
			SourceController = Source->AbilityActorInfo->PlayerController.Get();
			if (!SourceController && !SourceActor)
			{
				if (const APawn* Pawn = StaticCast<APawn*>(SourceActor))
				{
					SourceController = Pawn->GetController();
				}
			}

			if (SourceController)
			{
				SourceCharacter = StaticCast<ABaseCharacter*>(SourceController->GetPawn());
			}
			else
			{
				SourceCharacter = StaticCast<ABaseCharacter*>(SourceActor);
			}

			if (ContextHandle.GetEffectCauser())
			{
				SourceActor = ContextHandle.GetEffectCauser();
			}
		}

		// Try to extreact a hit result
		FHitResult HitResult;
		if (ContextHandle.GetHitResult())
		{
			HitResult = *ContextHandle.GetHitResult();
		}

		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);

		if (LocalDamageDone > 0.f)
		{
			// Apply the health change and the clamp it
			const float OldHealth = GetHealth();
			SetHealth(FMath::Clamp(OldHealth - LocalDamageDone, 0.f, GetMaxHealth()));
			if (TargetCharacter)
			{
				// This is proper damage
				TargetCharacter->HandleDamage(LocalDamageDone, HitResult, SourceTags, SourceCharacter, SourceActor);

				// Call for all health changes
				TargetCharacter->HandleHealthChanged(-LocalDamageDone, SourceTags);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));

		if (TargetCharacter)
		{
			// Call for all health changes
			TargetCharacter->HandleHealthChanged(Delta, SourceTags);
		}
	}
}

void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
}

void UBaseAttributeSet::AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
	const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const
{
	UAbilitySystemComponent* AbilitySystemComponent = GetOwningAbilitySystemComponent();

	const float CurMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurMaxValue, NewMaxValue) && AbilitySystemComponent)
	{
		const float CurValue = AffectedAttribute.GetCurrentValue();
		const float NewDelta = (CurMaxValue > 0) ?
			(CurValue * NewMaxValue / CurMaxValue) - CurValue :
			NewMaxValue;
		
		AbilitySystemComponent->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UBaseAttributeSet::OnRep_Health(const FGameplayAttributeData& OldData)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Health, OldData)
}

void UBaseAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldData)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxHealth, OldData)
}

void UBaseAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldData)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Damage, OldData)
}

void UBaseAttributeSet::OnRep_SpeedMultiplier(const FGameplayAttributeData& OldData)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, SpeedMultiplier, OldData)
}
