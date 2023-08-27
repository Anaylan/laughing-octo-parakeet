#pragma once

#include "CoreMinimal.h"
#include "AbilityEnumLibrary.generated.h"

UENUM(BlueprintType)
enum class EAbilityTypeInputID: uint8
{
	None,
	Confirm,
	Canceled,
	IA_NormalAttack UMETA(DisplayName="NormalAttack"),
	IA_Sprint UMETA(DisplayName="Sprint"),
	IA_Jump UMETA(DisplayName="Jump"),
	IA_ChargeAttack UMETA(DisplayName="ChargeAttack"),
	IA_JumpAttack UMETA(DisplayName="JumpAttack")
};

UENUM(BlueprintType)
enum class ESlotAbilityID: uint8
{
	NormalAttack,
	ChargeAttack
};
