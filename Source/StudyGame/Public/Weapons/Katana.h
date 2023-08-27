// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "Engine/DamageEvents.h"
#include "Katana.generated.h"

UCLASS()
class STUDYGAME_API AKatana : public ABaseWeapon
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDamageEvent DamageType;
public:
	// Sets default values for this actor's properties
	AKatana();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
