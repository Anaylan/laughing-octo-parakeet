// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/BaseCharacter.h"
#include "Library/AnimationStructLibrary.h"
#include "Library/WeaponStructLibrary.h"
#include "Library/CharacterStructLibrary.h"
#include "CharacterAnimInstance.generated.h"


class UIKFootComponent;
class ABaseCharacter;
class ABaseWeapon;
/**
 * 
 */


UCLASS()
class STUDYGAME_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Information")
	float ElapsedDelayTime = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Movement Information")
	float RotationScale = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement transition | Settings")
	float DynamicTriggerDelayL = .1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement transition | Settings")
	float DynamicTriggerDelayR = .1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Information")
	uint8 bJumped:1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Information")
	uint8 bMoving:1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Information")
	uint8 bCanPlayDynamicTransition:1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Information")
	float Direction = 0.f;

	/*************
	 * References
	 ************/
	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<UIKFootComponent> IKFootComponent = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<ABaseCharacter> CharacterOwner = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<UBaseMovementComponent> MovementComponent = nullptr;
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly, Category = "Curve")
	TObjectPtr<UCurveFloat> WalkCurveFloat = nullptr;
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly, Category = "Curve")
	TObjectPtr<UCurveFloat> RunCurveFloat = nullptr;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ABaseWeapon> CurrentWeapon = nullptr;
	
	FTimerHandle JumpedTimerHandle;
	FTimerHandle DynamicTransitionTimerHandle;

	/*******************
	 * Character state
	 ******************/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterInformation CharacterInformation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMovementProfile MovementProfile = EMovementProfile::Running;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMovementMode MovementMode = ELocomotionMode::Grounded;

	/*******************
	 * ???????????????
	 ******************/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIKFootProperties IKFootProperties;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFootLockProperties FootLockProperties;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIKWeaponProperties IKWeaponProperties;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLeanBlend LeanBlend;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWalkRunBlend WalkRunBlend;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAnimPlayRate AnimPlayRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMovementDirectionBlend DirectionBlend;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLayerBlend LayerBlend;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D AimingAngle = FVector2D::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform CameraTransform = FTransform();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform RelativeCameraTransform = FTransform();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement transition | Params")
	FTurnInPlaceAsset TurnIPL90;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement transition | Params")
	FTurnInPlaceAsset TurnIPR90;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement transition | Params")
	FTurnInPlaceAsset TurnIPL180;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement transition | Params")
	FTurnInPlaceAsset TurnIPR180;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement transition | Params")
	FDynamicMontageParams DynamicTurnIPL;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement transition | Params")
	FDynamicMontageParams DynamicTurnIPR;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EMovementDirection MovementDirection = EMovementDirection::Forward;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERotationMode RotationMode = ERotationMode::Looking;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EOverlayState OverlayState = EOverlayState::Default;
public:
	UCharacterAnimInstance();
	
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	/***********************************
	 * Skeletal name bones and notifies
	 **********************************/
	inline static FName NAME_RotationAmount = FName(TEXT("RotationAmount"));
	inline static FName NAME_Grounded = FName(TEXT("Grounded"));
	inline static FName NAME_EnableTransition = FName(TEXT("Enable_Transition"));
	inline static FName NAME_WeightGait = FName(TEXT("Weight_Gait"));
	inline static FName NAME_RootBone = FName(TEXT("root"));
	inline static FName NAME_HandIK_Root = FName(TEXT("ik_hand_root"));
	inline static FName NAME_FootIK_L = FName(TEXT("ik_foot_l"));
	inline static FName NAME_FootIK_R = FName(TEXT("ik_foot_r"));
	inline static FName NAME_FootLock_L = FName(TEXT("FootLock_L"));
	inline static FName NAME_FootLock_R = FName(TEXT("FootLock_R"));
	inline static FName NAME_VBFoot_L = FName(TEXT("VB foot_target_l"));
	inline static FName NAME_VBFoot_R = FName(TEXT("VB foot_target_r"));
	inline static FName NAME_EnableFootIK_L = FName(TEXT("Enable_FootIK_L"));
	inline static FName NAME_EnableFootIK_R = FName(TEXT("Enable_FootIK_R"));
	inline static FName NAME_LayerHandL = FName(TEXT("Layering_Hand_L"));
	inline static FName NAME_LayerHandR = FName(TEXT("Layering_Hand_R"));
	inline static FName NAME_LayerArmL_MS = FName(TEXT("Layering_Arm_L_MS"));
	inline static FName NAME_LayerArmR_MS = FName(TEXT("Layering_Arm_R_MS"));
	inline static FName NAME_LayerArmL_LS = FName(TEXT("Layering_Arm_L_LS"));
	inline static FName NAME_LayerArmR_LS = FName(TEXT("Layering_Arm_R_LS"));
	inline static FName NAME_LayerArmL_Add = FName(TEXT("Layering_Arm_L_Add"));
	inline static FName NAME_LayerArmR_Add = FName(TEXT("Layering_Arm_R_Add"));
	inline static FName NAME_LayerHead_Add = FName(TEXT("Layering_Head_Add"));
	inline static FName NAME_LayerSpine_Add = FName(TEXT("Layering_Spine_Add"));
	
protected:
	UFUNCTION(BlueprintCallable)
	bool CanTurnInPlace();
	void TurnInPlaceCheck(float DeltaTime);
	void TurnInPlace();

	UFUNCTION(BlueprintCallable)
	bool CanDynamicTransition();
	void DynamicTransitionCheck();
	
	void SetFootLocking(float DeltaTime, FName EnableCurveName, FName IKFootBone, FName LockCurve, FVector& CurFootLockLoc,
		FRotator& CurFootLockRot, float& CurFootLockAlpha);
	void SetFootLockOffset(float DeltaTime, FVector& CurFootLockLoc, FRotator& CurFootLockRot);
	virtual void CurrentWeaponChanged(ABaseWeapon* NewWeapon, const ABaseWeapon* OldWeapon);

	FVector CalculateRelativeAcceleration() const;
	
	// Update character information
	void UpdateEssentialValue();
	void UpdateWalkRunBlend(float DeltaSeconds);
	void UpdateLayerVariable();
	
	void CalculateWeaponSway(const float DeltaTime);
	FTransform CalculateRelativeCameraTransform();
	
	// Update grounded state
	void UpdateGrounded(float DeltaSeconds);
	
	// Methods for getting variables from UIKFootComponent
	void InitIK();
	void UpdateIK();

	void UpdateFootLock(FName FootSocket);

	UFUNCTION(BlueprintCallable)
	void PlayTransition(const FDynamicMontageParams& Params);
	UFUNCTION(BlueprintCallable)
	void PlayDynamicTransition(const FDynamicMontageParams& Params, float TriggerDelay);
	
	UFUNCTION(BlueprintCallable)
	void OnJumped();
	
	float GetAnimCurveValue(FName CurveName, float Bias, float Min, float Max) const;
	float CalculateStrideVelocity() const;
	float CalculateWalkRun() const;
	float CalculateStandingPlayRate() const;
	FVector2D CalculateAimingAngle() const;
	EMovementDirection CalculateDirection() const;

	void CalculateDirectionBlend(FMovementDirectionBlend& CurBlend, float DeltaTime, float InterpSpeed);
};
