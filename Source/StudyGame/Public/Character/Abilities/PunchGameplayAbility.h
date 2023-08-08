// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Abilities/BaseGameplayAbility.h"
#include "Library/WeaponEnumLibrary.h"
#include "Weapons/BaseWeapon.h"
#include "PunchGameplayAbility.generated.h"


/**
 * 
 */
UCLASS()
class STUDYGAME_API UPunchGameplayAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EWeaponType, UAnimMontage*> MontageSettings;
	
public:

	UPunchGameplayAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(Server, Reliable)
	void CheckOverlap(AActor* SourceActor, const ABaseWeapon* SourceWeapon);
};

