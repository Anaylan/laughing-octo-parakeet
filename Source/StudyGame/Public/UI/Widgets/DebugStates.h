// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/BaseWidget.h"
#include "DebugStates.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class STUDYGAME_API UDebugStates : public UBaseWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> MovementStates = nullptr;

public:

	void UpdateMovementStates() const;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
};
