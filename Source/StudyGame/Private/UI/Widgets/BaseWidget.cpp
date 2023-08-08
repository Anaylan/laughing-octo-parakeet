// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/BaseWidget.h"

#include "Animation/CharacterAnimInstance.h"

void UBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	CachedCharacter = StaticCast<ABaseCharacter*>(GetOwningPlayerPawn());

	if (!CachedCharacter.IsValid()) return;

	AnimInstance = StaticCast<UCharacterAnimInstance*>(CachedCharacter->GetMesh()->GetAnimInstance());

	if (!AnimInstance) return;
}

void UBaseWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (IsRunningDedicatedServer()) return;
}
