// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "BaseAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class STUDYGAME_API UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health{ 100.f };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth{ 100.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Damage)
	FGameplayAttributeData Damage{ 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_SpeedMultiplier)
	FGameplayAttributeData SpeedMultiplier{ 1.f };
public:

	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Damage);
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, SpeedMultiplier);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	void AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
		const FGameplayAttributeData& MaxAttribute,
		float NewMaxValue,
		const FGameplayAttribute& AffectedAttributeProperty) const;
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldData);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldData);

	UFUNCTION()
	void OnRep_Damage(const FGameplayAttributeData& OldData);

	UFUNCTION()
	void OnRep_SpeedMultiplier(const FGameplayAttributeData& OldData);
};
