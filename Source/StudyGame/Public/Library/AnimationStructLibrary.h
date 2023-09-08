// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationStructLibrary.generated.h"

USTRUCT(BlueprintType)
struct FDynamicMontageParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Transition")
	TObjectPtr<UAnimSequenceBase> Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Transition")
	float BlendInTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Transition")
	float BlendOutTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Transition")
	float PlayRate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Transition")
	float StartTime = 0.0f;
};

USTRUCT(BlueprintType)
struct FTurnInPlaceAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	TObjectPtr<UAnimSequenceBase> Animation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	float AnimatedAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	FName SlotName = FName(TEXT("Turn/Rotate"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	float PlayRate = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	bool ScaleTurnAngle = true;
};

USTRUCT(BlueprintType)
struct FWalkRunBlend
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Stride = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WalkRun = 0.f;
};

USTRUCT(BlueprintType)
struct FAnimPlayRate
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Standing")
	float AnimWalkSpeed = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Standing")
	float AnimRunSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Standing")
	float AnimSprintSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Play Rate")
	float JumpPlayRate = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Play Rate")
	float StandingPlayRate = 1.f;
};

USTRUCT(BlueprintType)
struct FLeanBlend
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Params")
	float FB = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Params")
	float LR = 0.f;
};

USTRUCT(BlueprintType)
struct FMovementDirectionBlend
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Params")
	float F = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Params")
	float R = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Params")
	float B = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Params")
	float L = 0.f;
};

USTRUCT(BlueprintType)
struct FLayerBlend
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HandL = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HandR = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ArmL_LS = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ArmR_LS = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ArmL_MS = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ArmR_MS = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ArmL_Add = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ArmR_Add = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Head_Add = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Spine_Add = 0.f;
};

USTRUCT(BlueprintType)
struct FMontageDirection
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> Forward;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> Right;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> Backward;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> Left;
};