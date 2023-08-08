#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "Library/WeaponStructLibrary.h"
#include "WeaponGameplayAbility.generated.h"

UCLASS()
class UWeaponGameplayAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EWeaponType, UAnimMontage*> MontageSettings;
};