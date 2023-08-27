// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/BaseDamageEffect.h"
#include "GameplayTagsManager.h"
#include "..\..\Public\Character\Attributes\BaseAttributeSet.h"

UBaseDamageEffect::UBaseDamageEffect()
{
	FGameplayModifierInfo DamageInfo;
	DamageInfo.Attribute = UBaseAttributeSet::GetDamageAttribute();
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