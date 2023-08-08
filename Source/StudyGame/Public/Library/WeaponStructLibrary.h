// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponEnumLibrary.h"
#include "WeaponStructLibrary.generated.h"

USTRUCT(BlueprintType)
struct FWeaponProperties
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseDMG = 0.f;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// float AttackRange = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DMGMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name = FName(TEXT(""));

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType Type = EWeaponType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponRange Range = EWeaponRange::Melee;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackSpeed = 1.f;
	
};

USTRUCT(BlueprintType)
struct FIKWeaponProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequence* AnimPose = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AimOffset = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform OffsetTransform;
};

USTRUCT(BlueprintType)
struct FWeaponMontageSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee")
	TObjectPtr<UAnimMontage> Sword;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee")
	TObjectPtr<UAnimMontage> Dagger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee")
	TObjectPtr<UAnimMontage> Spear;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee")
	TObjectPtr<UAnimMontage> Katana;
};