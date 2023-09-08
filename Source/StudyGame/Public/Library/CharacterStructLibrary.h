// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterEnumLibrary.h"
#include "Engine/DataTable.h"
#include "Curves/CurveVector.h"
#include "CharacterStructLibrary.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMovementProfileSettings
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed", meta = (AllowPrivateAccess = true, ClampMin="0", UIMin="0", ForceUnits="cm"))
	float WalkSpeed = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed", meta = (AllowPrivateAccess = true, ClampMin="0", UIMin="0", ForceUnits="cm"))
	float RunSpeed = 275.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed", meta = (AllowPrivateAccess = true, ClampMin="0", UIMin="0", ForceUnits="cm"))
	float SprintSpeed = 650.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curve", meta = (AllowPrivateAccess = true))
	TSoftObjectPtr<UCurveVector> MovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curve", meta = (AllowPrivateAccess = true))
	TSoftObjectPtr<UCurveFloat> RotationCurve;
public:

	UCurveVector* GetMovementCurve() const
	{
		return MovementCurve.LoadSynchronous();
	}

	UCurveFloat* GetRotationCurve() const
	{
		return RotationCurve.LoadSynchronous();
	}
	
	float GetSpeedForMovementProfile(EMovementProfile MovementProfile) const
	{
		switch (MovementProfile)
		{
			case EMovementProfile::Sprint:
				return SprintSpeed;
			case EMovementProfile::Running:
				return RunSpeed;
			case EMovementProfile::Walking:
				return WalkSpeed;
			default:
				return RunSpeed;
		}
	}
};

USTRUCT(BlueprintType)
struct FMovementProfile
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gait", meta = (AllowPrivateAccess = true))
	EMovementProfile Profile = EMovementProfile::Walking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gait", meta = (AllowPrivateAccess = true))
	bool bWalking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gait", meta = (AllowPrivateAccess = true))
	bool bRunning = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gait", meta = (AllowPrivateAccess = true))
	bool bSprint = false;

public:
	FMovementProfile() {}

	FMovementProfile(EMovementProfile InitialProfile)
	{
		Profile = InitialProfile;
	}

	void operator=(EMovementProfile NewProfile)
	{
		Profile = NewProfile;
		bWalking = Profile == EMovementProfile::Walking;
		bRunning = Profile == EMovementProfile::Running;
		bSprint = Profile == EMovementProfile::Sprint;
	}
	
	bool IsWalking() const
	{
		return bWalking;
	}

	bool IsRunning() const
	{
		return bRunning;
	}

	bool IsSprint() const
	{
		return bSprint;
	}
};

USTRUCT(BlueprintType)
struct FMovementMode
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gait", meta = (AllowPrivateAccess = true))
	ELocomotionMode MovementMode = ELocomotionMode::Grounded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Mode", meta = (AllowPrivateAccess = true))
	bool bIsGrounded = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Mode", meta = (AllowPrivateAccess = true))
	bool bIsFalling = false;

public:
	FMovementMode() {}

	FMovementMode(ELocomotionMode InitialMode)
	{
		MovementMode = InitialMode;
	}

	void operator=(ELocomotionMode NewProfile)
	{
		MovementMode = NewProfile;
		bIsGrounded = MovementMode == ELocomotionMode::Grounded;
		bIsFalling = MovementMode == ELocomotionMode::Falling;
	}
	
	bool IsFalling() const
	{
		return bIsFalling;
	}

	bool IsGrounded() const
	{
		return bIsGrounded;
	}
};

USTRUCT(BlueprintType)
struct FCharacterInformation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Information", meta = (AllowPrivateAccess = true))
	float Speed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Information", meta = (AllowPrivateAccess = true))
	FRotator ActorRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Information", meta = (AllowPrivateAccess = true))
	FRotator AimingRotation = FRotator::ZeroRotator;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Information", meta = (AllowPrivateAccess = true))
	FVector Velocity = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Information", meta = (AllowPrivateAccess = true))
	FVector Acceleration = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Information", meta = (AllowPrivateAccess = true))
	float AimYawRate = 0.f;
};

USTRUCT(BlueprintType)
struct FMovementSettings : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMovementProfileSettings LookingDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMovementProfileSettings VelocityDirection;
};

USTRUCT(BlueprintType)
struct FSlideSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float MinSpeed = 350.f;

	UPROPERTY(EditDefaultsOnly)
	float EnterImpulse = 500.f;

	UPROPERTY(EditDefaultsOnly)
	float GravityForce = 5000.f;

	UPROPERTY(EditDefaultsOnly)
	float Friction = 1.3f;
	
};

USTRUCT(BlueprintType)
struct FIKFootProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LeftFootOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RightFootOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PelvisOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fFootHeight = 13.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator LeftFootRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RightFootRotation = FRotator::ZeroRotator;
};

USTRUCT(BlueprintType)
struct FFootLockProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LeftFootLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RightFootLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LeftFootAlpha = .0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RightFootAlpha = .0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator LeftFootRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RightFootRotation = FRotator::ZeroRotator;
};

USTRUCT(BlueprintType)
struct FCharacterCurveNames
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName NAME_EnableTransition = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName NAME_EnableFootIK_R = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName NAME_EnableFootIK_L = NAME_None;
};

USTRUCT(BlueprintType)
struct FIKHandSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName LeftHandIK = FName(TEXT("ik_hand_l"));
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RightHandIK = FName(TEXT("ik_hand_r"));
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName WeaponSocketL = FName(TEXT("Weapon_L"));
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName WeaponSocketR = FName(TEXT("Weapon_R"));
};

USTRUCT(BlueprintType)
struct FIKHandValues
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector HandLocationL = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector HandLocationR = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator HandRotationR = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator HandRotationL = FRotator::ZeroRotator;
};