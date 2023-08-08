// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


/* Needed for describes pawn movement speed */
UENUM(BlueprintType, meta = (ScriptName="EMovementProfile"))
enum class EMovementProfile : uint8
{
	Walking,
	Running,
	Sprint
};

UENUM(BlueprintType)
enum class ELocomotionMode : uint8
{
	Grounded,
	Falling
};

UENUM(BlueprintType)
enum class EPawnGlobalStance : uint8
{
	Unarmed,
	Armed
};

UENUM(BlueprintType)
enum class EOverlayState : uint8
{
	Default,
	Katana,
	Sword
};

UENUM(BlueprintType)
enum class EMovementAction : uint8
{
	None,
	Mantling,
	Dashing,
};

UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	Looking,
	Velocity
};

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Forward,
	Right,
	Backward,
	Left
};

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_NONE,
	CMOVE_Slide,
	CMOVE_MAX
};