// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DebugStates.h"

#include "Character/BaseCharacter.h"
#include "Components/TextBlock.h"
#include "Engine/PackageMapClient.h"
#include "Library/StructEnumLibrary.h"

void UDebugStates::UpdateMovementStates() const
{
	if (!CachedCharacter.IsValid()) return;
	
	FString TargetString = UStructEnumLibrary::EnumToString<EMovementProfile>(CachedCharacter->GetMovementProfile());
	TargetString.Append("\n");
	TargetString.Append(UStructEnumLibrary::EnumToString<ERotationMode>(CachedCharacter->GetRotationMode()));
	TargetString.Append("\n");
	TargetString.Append(UStructEnumLibrary::EnumToString<EMovementAction>(CachedCharacter->GetMovementAction()));
	TargetString.Append("\n");
	TargetString.Append(UStructEnumLibrary::EnumToString<EPawnGlobalStance>(CachedCharacter->GetMGlobalStance()));
	TargetString.Append("\n");
	TargetString.Append(UStructEnumLibrary::EnumToString<ELocomotionMode>(CachedCharacter->GetLocomotionMode()));
	TargetString.Append("\n");
	TargetString.Append(UStructEnumLibrary::EnumToString<EMovementMode>(CachedCharacter->GetCharacterMovement()->MovementMode));
	TargetString.Append("\n");
	TargetString.Append(UStructEnumLibrary::EnumToString<EOverlayState>(CachedCharacter->GetOverlayState()));
	TargetString.Append("\n");
	// TargetString.Append(GetWorld()->GetNetDriver()->GuidCache->GetOrAssignNetGUID(CachedCharacter.Get()).ToString());
	
	MovementStates->SetText(FText::FromString(TargetString));
}

void UDebugStates::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateMovementStates();
}
