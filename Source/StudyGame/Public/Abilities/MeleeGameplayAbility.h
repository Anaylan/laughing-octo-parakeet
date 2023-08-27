#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "Library/WeaponStructLibrary.h"
#include "MeleeGameplayAbility.generated.h"

UCLASS()
class UMeleeGameplayAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EWeaponType, FMontageGlobalState> MontageSettings;
};