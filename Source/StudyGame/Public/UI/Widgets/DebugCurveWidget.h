// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/BaseWidget.h"
#include "DebugCurveWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class STUDYGAME_API UDebugCurveWidget : public UBaseWidget
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
		TObjectPtr<UTextBlock> CurveNames = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
		TObjectPtr<UTextBlock> CurveValues = nullptr;	
	
public:

	UFUNCTION(BlueprintCallable)
		void GetMovementCurveValues();

	UFUNCTION(BlueprintCallable)
		void GetMovementCurveNames();

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
