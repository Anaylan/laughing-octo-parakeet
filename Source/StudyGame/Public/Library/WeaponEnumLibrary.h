// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	NONE,
	Sword,
	Dagger,
	Spear,
	Katana
};

UENUM(BlueprintType)
enum class EWeaponRange : uint8
{
	NONE,
	Melee,
	Ranged
};