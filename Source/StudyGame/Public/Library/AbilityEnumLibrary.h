#pragma once

#include "CoreMinimal.h"
#include "AbilityEnumLibrary.generated.h"

UENUM(BlueprintType)
enum class EAbilityTypeInputID: uint8
{
	None,
	Confirm UMETA(Hidden),
	Canceled UMETA(Hidden),
	IA_NormalAttack UMETA(DisplayName="NormalAttack"),
	IA_Sprint UMETA(DisplayName="Sprint"),
	IA_Jump UMETA(DisplayName="Jump"),
	IA_ChargeAttack UMETA(DisplayName="ChargeAttack"),
	IA_JumpAttack UMETA(DisplayName="JumpAttack"),
	IA_Dash UMETA(DisplayName="Dash")
};