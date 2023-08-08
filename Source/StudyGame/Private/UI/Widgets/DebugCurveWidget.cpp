// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DebugCurveWidget.h"

#include "Animation/CharacterAnimInstance.h"
#include "Components/TextBlock.h"


void UDebugCurveWidget::GetMovementCurveNames()
{
	if (!IsValid(AnimInstance)) return;
	if (!CurveNames) return;
	
	TArray<FName> Curves;
	AnimInstance->GetAllCurveNames(Curves);
	AnimInstance->GetActiveCurveNames(EAnimCurveType::AttributeCurve, Curves);

	FString CurveString;
	
	for (FName Curve : Curves)
	{
		CurveString.Append(Curve.ToString());
		CurveString.Append("\n");
	}
	
	CurveNames->SetText(FText::FromString(CurveString));
}

void UDebugCurveWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	GetMovementCurveNames();
	GetMovementCurveValues();
}

void UDebugCurveWidget::GetMovementCurveValues()
{
	
	if (!IsValid(AnimInstance)) return;
	if (!CurveValues) return;
	
	TArray<FName> Curves;
	AnimInstance->GetAllCurveNames(Curves);
	AnimInstance->GetActiveCurveNames(EAnimCurveType::AttributeCurve, Curves);

	FString CurveString;
	
	for (FName Curve : Curves)
	{
		CurveString.Append(FString::Printf(TEXT("%f"), AnimInstance->GetCurveValue(Curve)));
		CurveString.Append("\n");
	}
	
	CurveValues->SetText(FText::FromString(CurveString));
}
