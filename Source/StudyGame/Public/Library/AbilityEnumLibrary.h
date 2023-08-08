#pragma once

#include "CoreMinimal.h"
#include "AbilityEnumLibrary.generated.h"

UENUM(BlueprintType)
enum class EAbilityTypeInputID: uint8
{
	None,
	Confirm,
	Canceled,
	IA_NormalAttack UMETA(DisplayName="NormalAttack")
};