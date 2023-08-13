// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_SuccessFailEvent.generated.h"

class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSuccessFailEventDelegate, FGameplayEventData, Payload);
/**
 * Custom Ability Task which uses a Success or Failed tag to decide between two branches.
 */
UCLASS()
class STUDYGAME_API UAbilityTask_SuccessFailEvent : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FSuccessFailEventDelegate SuccessEventReceived;
	UPROPERTY(BlueprintAssignable)
	FSuccessFailEventDelegate FailedEventReceived;
	
	/**
	 * Wait until the specified gameplay tags event's trigger. Allows success or failure.
	 * By default this will look at the owner of this ability.
	 * OptionalExternalTarget can be set to make this look at another actor's tags for changes.
	 * It will keep listening as long as OnlyTriggerOnce = false
	 * If OnlyMatchExact = false it will trigger for nested tags
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility",
		DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_SuccessFailEvent* WaitSuccessFailEvent(UGameplayAbility* OwningAbility, FGameplayTag SuccessTag,
		FGameplayTag FailedTag, AActor* OptionalExternalTarget=nullptr, bool OnlyTriggerOnce=false, bool OnlyMatchExact = true);

	void SetExternalTarget(AActor* Actor);

	UAbilitySystemComponent* GetTargetASC();

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void SuccessEventCallback(const FGameplayEventData* Payload);
	virtual void FailedEventCallback(const FGameplayEventData* Payload);
	virtual void SuccessEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload);
	virtual void FailedEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload);

	virtual void OnDestroy(bool AbilityEnding) override;

	FGameplayTag SuccessTag;
	FGameplayTag FailedTag;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OptionalExternalTarget;

	bool UseExternalTarget;	
	bool OnlyTriggerOnce;
	bool OnlyMatchExact;

	FDelegateHandle SuccessHandle;
	FDelegateHandle FailedHandle;
};
