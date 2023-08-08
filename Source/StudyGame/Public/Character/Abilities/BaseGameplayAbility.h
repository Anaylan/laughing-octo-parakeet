// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Library/AbilityEnumLibrary.h"
#include "BaseGameplayAbility.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STUDYGAME_API UBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EAbilityTypeInputID AbilityTypeID = EAbilityTypeInputID::None;
};


