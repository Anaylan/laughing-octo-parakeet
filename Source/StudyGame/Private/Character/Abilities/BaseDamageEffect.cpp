// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/BaseDamageEffect.h"
#include "GameplayTagsManager.h"
#include "Character/Attributes/HealthAttributeSet.h"

UBaseDamageEffect::UBaseDamageEffect()
{
	FGameplayModifierInfo DamageInfo;
	DamageInfo.Attribute = UHealthAttributeSet::GetDamageAttribute();
	DamageInfo.ModifierOp = EGameplayModOp::Override;
	DamageInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(40.f);
	Modifiers.Add(DamageInfo);

	FGameplayEffectCue DamageCue;
	FGameplayTagContainer DamageTagContainer;
	TArray<FString> Tags;
	Tags.Add("GameplayCue.Attacked");
	UGameplayTagsManager::Get().RequestGameplayTagContainer(Tags, DamageTagContainer);
	DamageCue.GameplayCueTags = DamageTagContainer;
	GameplayCues.Add(DamageCue);
}