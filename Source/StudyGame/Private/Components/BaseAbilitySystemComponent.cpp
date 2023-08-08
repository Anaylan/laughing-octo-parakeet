// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BaseAbilitySystemComponent.h"

#include "AbilitySystemGlobals.h"

UBaseAbilitySystemComponent* UBaseAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor,
                                                                                             bool LookForComponent)
{
	return StaticCast<UBaseAbilitySystemComponent*>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor,
		LookForComponent));
}
