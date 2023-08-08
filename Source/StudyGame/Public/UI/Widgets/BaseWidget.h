// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseWidget.generated.h"

class UCharacterAnimInstance;
class ABaseCharacter;
/**
 * 
 */
UCLASS()
class STUDYGAME_API UBaseWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (AllowPrivateAccess = true))
	TObjectPtr<UCharacterAnimInstance> AnimInstance = nullptr;

	UPROPERTY(meta = (AllowPrivateAccess = true))
	TWeakObjectPtr<ABaseCharacter> CachedCharacter = nullptr;
	
public:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
