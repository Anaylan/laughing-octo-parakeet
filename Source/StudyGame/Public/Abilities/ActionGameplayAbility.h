// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BaseGameplayAbility.h"
#include "Library/AnimationStructLibrary.h"
#include "ActionGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class STUDYGAME_API UActionGameplayAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMontageDirection ActionBlend;
};
