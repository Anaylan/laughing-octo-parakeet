// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CharacterAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/IKFootComponent.h"
#include "Character/BaseCharacter.h"
#include "Weapons/BaseWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"

UCharacterAnimInstance::UCharacterAnimInstance()
{
	bJumped = false;
	bMoving = false;
	bCanPlayDynamicTransition = true;
}

void UCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	CharacterOwner = StaticCast<ABaseCharacter*>(TryGetPawnOwner());
	if (!CharacterOwner.IsValid()) { return; }
	bCanPlayDynamicTransition = true;
	
	MovementComponent = CharacterOwner->FindComponentByClass<UBaseMovementComponent>();
	
	CharacterOwner->JumpedDelegate.AddDynamic(this, &UCharacterAnimInstance::OnJumped);
	
	InitIK();
}

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	CharacterOwner = StaticCast<ABaseCharacter*>(TryGetPawnOwner());
	if (!CharacterOwner.IsValid()) { return; } 
	
	InitIK();
}

void UCharacterAnimInstance::UpdateWalkRunBlend(float DeltaSeconds)
{
	const float TargetStrideVelocity = CalculateStrideVelocity();
	const float TargetWalkRun = CalculateWalkRun();
	const float BlendInterpSpeed = 6.f;
	
	WalkRunBlend.WalkRun = FMath::FInterpTo(WalkRunBlend.WalkRun, TargetWalkRun, DeltaSeconds, 
								BlendInterpSpeed);
	WalkRunBlend.Stride = FMath::FInterpTo(WalkRunBlend.Stride, TargetStrideVelocity, DeltaSeconds, 
								BlendInterpSpeed);
}

void UCharacterAnimInstance::UpdateLayerVariable()
{
	LayerBlend.HandL = GetCurveValue(NAME_LayerHandL);
	LayerBlend.HandR = GetCurveValue(NAME_LayerHandR);

	LayerBlend.ArmL_LS = GetCurveValue(NAME_LayerArmL_LS);
	LayerBlend.ArmR_LS = GetCurveValue(NAME_LayerArmR_LS);

	LayerBlend.ArmR_Add = GetCurveValue(NAME_LayerArmR_Add);
	LayerBlend.ArmL_Add = GetCurveValue(NAME_LayerArmL_Add);

	LayerBlend.Head_Add = GetCurveValue(NAME_LayerHead_Add);
	LayerBlend.Spine_Add = GetCurveValue(NAME_LayerSpine_Add);
	
	LayerBlend.ArmL_MS = 1 - FMath::Floor(GetCurveValue(NAME_LayerArmL_LS));
	LayerBlend.ArmR_MS = 1 - FMath::Floor(GetCurveValue(NAME_LayerArmR_LS));
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!CharacterOwner.IsValid()) { return; } 
	
	UpdateIK();
	UpdateEssentialValue();
	UpdateLayerVariable();
	
	const FVector TargetLeanVector = CalculateRelativeAcceleration();
	LeanBlend.FB = FMath::FInterpTo(LeanBlend.FB, TargetLeanVector.X, DeltaSeconds, 6.f);
	LeanBlend.LR = FMath::FInterpTo(LeanBlend.LR, TargetLeanVector.Y, DeltaSeconds, 6.f);
	
	CalculateWeaponSway(DeltaSeconds);
	
	if (MovementMode.IsGrounded())
	{
		UpdateGrounded(DeltaSeconds);

		// TODO: Implement rotation in place
		if (!bMoving)
		{
			if (CanTurnInPlace())
			{
				TurnInPlaceCheck(DeltaSeconds);
			}
			else if (CanDynamicTransition())
			{
				DynamicTransitionCheck();
			}
		}

		SetFootLocking(DeltaSeconds, NAME_EnableFootIK_L, NAME_FootIK_L, NAME_FootLock_L,
			FootLockProperties.LeftFootLocation, FootLockProperties.LeftFootRotation, 
			FootLockProperties.LeftFootAlpha);

		SetFootLocking(DeltaSeconds, NAME_EnableFootIK_R, NAME_FootIK_R, NAME_FootLock_R,
			FootLockProperties.RightFootLocation, FootLockProperties.RightFootRotation, 
			FootLockProperties.RightFootAlpha);
		
		MovementDirection = CalculateDirection();
		
		CalculateDirectionBlend(DirectionBlend, DeltaSeconds, 10.f);
	}
}

bool UCharacterAnimInstance::CanTurnInPlace()
{
	return GetCurveValue(NAME_EnableTransition) >= .99f && RotationMode == ERotationMode::Velocity;
}

void UCharacterAnimInstance::TurnInPlaceCheck(float DeltaTime)
{
	if (FMath::Abs(AimingAngle.X) > 45.f && CharacterInformation.AimYawRate > 50.f)
	{
		ElapsedDelayTime += DeltaTime;
	}
	else
	{
		ElapsedDelayTime = 0.f;
		return;
	}
	
	const float ClampedAimingX = FMath::GetMappedRangeValueClamped<float, float>({45.f, 180.f},
		{0.75f, 0.f}, FMath::Abs(AimingAngle.X));
	
	if (ElapsedDelayTime > ClampedAimingX)
	{		
		TurnInPlace();
	}
}

void UCharacterAnimInstance::TurnInPlace()
{
	FRotator DeltaRotator = CharacterInformation.AimingRotation - CharacterInformation.ActorRotation;
	DeltaRotator.Normalize();
	float const TurnAngle = DeltaRotator.Yaw;
	
	FTurnInPlaceAsset TargetAsset;
	
	if (FMath::Abs(TurnAngle) < 120.f)
	{
		TargetAsset = TurnAngle < 0.f ? TurnIPL90 : TurnIPR90;
	}
	else
	{
		TargetAsset = TurnAngle < 0.f ? TurnIPL180 : TurnIPR180;
	}

	if (IsPlayingSlotAnimation(TargetAsset.Animation, TargetAsset.SlotName)) return;

	PlaySlotAnimationAsDynamicMontage(TargetAsset.Animation, TargetAsset.SlotName, .2f, .2f,
		TargetAsset.PlayRate, 1, 0.f, 0.f);
	
	RotationScale = TargetAsset.ScaleTurnAngle ? (TurnAngle / TargetAsset.AnimatedAngle) * TargetAsset.PlayRate :
		TargetAsset.PlayRate;

}

bool UCharacterAnimInstance::CanDynamicTransition()
{
	return GetCurveValue(NAME_EnableTransition) >= 1.f;
}

void UCharacterAnimInstance::DynamicTransitionCheck()
{
	FTransform SocketTransformA = GetOwningComponent()->GetSocketTransform(NAME_FootIK_L, RTS_Component);
	FTransform SocketTransformB = GetOwningComponent()->GetSocketTransform(NAME_VBFoot_L, RTS_Component);
	float Delta = static_cast<float>((SocketTransformB.GetLocation() - SocketTransformA.GetLocation()).Size());
	if (Delta > 8.f)
	{
		PlayDynamicTransition(DynamicTurnIPL, DynamicTriggerDelayL);
		return;
	}
	
	SocketTransformA = GetOwningComponent()->GetSocketTransform(NAME_FootIK_R, RTS_Component);
	SocketTransformB = GetOwningComponent()->GetSocketTransform(NAME_VBFoot_R, RTS_Component);
	Delta = static_cast<float>((SocketTransformB.GetLocation() - SocketTransformA.GetLocation()).Size());
	if (Delta > 8.f)
	{
		PlayDynamicTransition(DynamicTurnIPR, DynamicTriggerDelayR);
	}
}

void UCharacterAnimInstance::SetFootLocking(float DeltaTime, FName EnableCurveName, FName IKFootBone, FName LockCurve,
                                            FVector& CurFootLockLoc, FRotator& CurFootLockRot, float& CurFootLockAlpha)
{
	if (GetCurveValue(EnableCurveName) <= .0f) return;
	
	float CurveValue = GetCurveValue(LockCurve);
	
	if (CurveValue >= .99f || CurveValue < CurFootLockAlpha)
	{
		CurFootLockAlpha = CurveValue;
	}

	if (CurFootLockAlpha >= .99f)
	{
		const FTransform FootTransform = GetOwningComponent()->GetSocketTransform(IKFootBone, ERelativeTransformSpace::RTS_Component);
		CurFootLockLoc = FootTransform.GetLocation();
		CurFootLockRot = FootTransform.Rotator();
	}
	
	if (CurFootLockAlpha > 0.f)
	{
		SetFootLockOffset(DeltaTime, CurFootLockLoc, CurFootLockRot);
	}
	
}

void UCharacterAnimInstance::SetFootLockOffset(float DeltaTime, FVector& CurFootLockLoc, FRotator& CurFootLockRot)
{
	if (!MovementComponent.IsValid() || !MovementComponent->IsMovingOnGround()) return;

	FRotator RotationDiff = CharacterInformation.ActorRotation - MovementComponent->GetLastUpdateRotation();
	RotationDiff.Normalize();

	FVector LocationDiff = GetOwningComponent()->GetComponentRotation().UnrotateVector(
		CharacterInformation.Velocity * DeltaTime);
	
	CurFootLockLoc = (CurFootLockLoc - LocationDiff).RotateAngleAxis(RotationDiff.Yaw, FVector::DownVector);
	
	CurFootLockRot -= RotationDiff;
	CurFootLockRot.Normalize();
}

void UCharacterAnimInstance::CurrentWeaponChanged(ABaseWeapon* NewWeapon, const ABaseWeapon* OldWeapon)
{
	CurrentWeapon = NewWeapon;
	if (CurrentWeapon)
	{
		// IKWeaponProperties = CurrentWeapon->GetIKProperties();
	}
}

void UCharacterAnimInstance::UpdateEssentialValue()
{
	CharacterInformation.Acceleration = CharacterOwner->GetAcceleration();
	CharacterInformation.ActorRotation = CharacterOwner->GetActorRotation();
	CharacterInformation.AimingRotation = CharacterOwner->GetAimingRotation();
	CharacterInformation.Velocity = CharacterOwner->GetVelocity();
	CharacterInformation.Speed = CharacterOwner->GetSpeed();
	CharacterInformation.AimYawRate = CharacterOwner->GetAimYaw();
	
	bMoving = CharacterOwner->IsMoving();

	// RelativeCameraTransform = CalculateRelativeCameraTransform();

	MovementMode = CharacterOwner->GetLocomotionMode();

	AimingAngle = CalculateAimingAngle();

	RotationMode = CharacterOwner->GetRotationMode();
	OverlayState = CharacterOwner->GetOverlayState();
}

float UCharacterAnimInstance::GetAnimCurveValue(FName CurveName, float Bias, float Min, float Max) const
{
	return FMath::Clamp(GetCurveValue(CurveName) + Bias, Min, Max);
}

void UCharacterAnimInstance::UpdateGrounded(float DeltaSeconds)
{
	MovementProfile = CharacterOwner->GetMovementProfile();

	UpdateWalkRunBlend(DeltaSeconds);
	AnimPlayRate.StandingPlayRate = CalculateStandingPlayRate();
	
	Direction = UKismetAnimationLibrary::CalculateDirection(CharacterInformation.Velocity,
		CharacterInformation.ActorRotation);
}

void UCharacterAnimInstance::InitIK()
{
	const AActor* Owner = TryGetPawnOwner();
	if (!IsValid(Owner)) return;
	
	UActorComponent* ActorComponent = Owner->GetComponentByClass(UIKFootComponent::StaticClass());
	if (!IsValid(ActorComponent)) return;
	
	IKFootComponent = StaticCast<UIKFootComponent*>(ActorComponent);
}

void UCharacterAnimInstance::UpdateIK()
{
	if (!IKFootComponent.IsValid()) return;

	IKFootProperties = IKFootComponent->GetIKProperties();
}

void UCharacterAnimInstance::UpdateFootLock(FName FootSocket)
{
}

void UCharacterAnimInstance::PlayTransition(const FDynamicMontageParams& Params)
{
	PlaySlotAnimationAsDynamicMontage(Params.Animation, NAME_Grounded, Params.BlendInTime,
		Params.BlendOutTime, Params.PlayRate, 1, .0f, Params.StartTime);
}

void UCharacterAnimInstance::PlayDynamicTransition(const FDynamicMontageParams& Params, const float TriggerDelay)
{
	if (bCanPlayDynamicTransition)
	{
		bCanPlayDynamicTransition = false;
		
		PlayTransition(Params);
		
		GetWorld()->GetTimerManager().SetTimer(DynamicTransitionTimerHandle, [this]() { bCanPlayDynamicTransition = true; },
			TriggerDelay, false);
	}
}

void UCharacterAnimInstance::OnJumped()
{
	bJumped = true;

	GetWorld()->GetTimerManager().SetTimer(JumpedTimerHandle, [this]() { bJumped = false; },
		.1f, false);
}

#pragma region CalculateFunctions
void UCharacterAnimInstance::CalculateWeaponSway(const float DeltaTime)
{
}

float UCharacterAnimInstance::CalculateStrideVelocity() const
{
	if (WalkCurveFloat && RunCurveFloat)
	{
		const float CurveTime = CharacterInformation.Speed / GetOwningComponent()->GetComponentScale().Z;

		const float ClampedMovementProfile = GetAnimCurveValue(NAME_WeightGait, -2.f, 0.f, 1.f);
			
		return FMath::Lerp(WalkCurveFloat->GetFloatValue(CurveTime), RunCurveFloat->GetFloatValue(CurveTime),
			ClampedMovementProfile);
	}
	return 0.f;
}

float UCharacterAnimInstance::CalculateWalkRun() const
{
	return MovementProfile.IsRunning() ? 1.f : 0.f;
}

float UCharacterAnimInstance::CalculateStandingPlayRate() const
{
	const float LerpedRun = FMath::Lerp(CharacterInformation.Speed / AnimPlayRate.AnimWalkSpeed,
										CharacterInformation.Speed / AnimPlayRate.AnimRunSpeed,
										GetAnimCurveValue(NAME_WeightGait, -1.f, 0.f, 1.f));

	const float LerpedSprint = FMath::Lerp(LerpedRun,
											CharacterInformation.Speed / AnimPlayRate.AnimSprintSpeed,
											GetAnimCurveValue(NAME_WeightGait, -2.f, 0.f, 1.f));
	
	return  FMath::Clamp((LerpedSprint / WalkRunBlend.Stride) / GetOwningComponent()->GetComponentScale().Z, 0.f, 10.f);
}

FVector2D UCharacterAnimInstance::CalculateAimingAngle() const
{
	FRotator AngleRotation = CharacterInformation.AimingRotation - CharacterInformation.ActorRotation;
	AngleRotation.Normalize();
	return {AngleRotation.Yaw, AngleRotation.Pitch};
}

EMovementDirection UCharacterAnimInstance::CalculateDirection() const
{
	if (MovementProfile.IsSprint() || RotationMode == ERotationMode::Looking)
	{
		return EMovementDirection::Forward;
	}

	if (Direction < 70.f && Direction > -70.f)
	{
		return EMovementDirection::Forward;
	}
	else if (Direction >= 70.f && Direction < 130.f)
	{
		return EMovementDirection::Right;
	}
	else if (Direction >= 130.f || Direction <= -130.f)
	{
		return EMovementDirection::Backward;
	}
	else if (Direction > -130.f && Direction <= -70.f)
	{
		return EMovementDirection::Left;
	}
	
	return EMovementDirection::Forward;
}

void UCharacterAnimInstance::CalculateDirectionBlend(FMovementDirectionBlend& CurBlend, float DeltaTime, float InterpSpeed)
{
	FMovementDirectionBlend TargetBlend;

	FVector const RelVelocity = CharacterInformation.ActorRotation.UnrotateVector(CharacterInformation.Velocity.GetSafeNormal());
	float const Sum = FMath::Abs(RelVelocity.X) + FMath::Abs(RelVelocity.Y) + FMath::Abs(RelVelocity.Z);

	FVector const RelDirection = RelVelocity / Sum;
	
	TargetBlend.F = FMath::Clamp(RelDirection.X, 0.f, 1.f);
	TargetBlend.L = FMath::Abs(FMath::Clamp(RelDirection.Y, -1.f, 0.f));
	TargetBlend.B = FMath::Abs(FMath::Clamp(RelDirection.X, -1.f, 0.f));
	TargetBlend.R = FMath::Clamp(RelDirection.Y, 0.f, 1.f);
	
	CurBlend.F = FMath::FInterpTo(CurBlend.F, TargetBlend.F, DeltaTime, InterpSpeed);
	CurBlend.L = FMath::FInterpTo(CurBlend.L, TargetBlend.L, DeltaTime, InterpSpeed);
	CurBlend.B = FMath::FInterpTo(CurBlend.B, TargetBlend.B, DeltaTime, InterpSpeed);
	CurBlend.R = FMath::FInterpTo(CurBlend.R, TargetBlend.R, DeltaTime, InterpSpeed);
}

FTransform UCharacterAnimInstance::CalculateRelativeCameraTransform()
{
	CameraTransform = FTransform(CharacterOwner->GetBaseAimRotation(),
	CharacterOwner->GetFollowCamera()->GetComponentLocation());
	
	const FTransform& RootOffset = CharacterOwner->GetMesh()->GetSocketTransform(NAME_RootBone,
	RTS_Component).Inverse() * CharacterOwner->GetMesh()->GetSocketTransform(NAME_HandIK_Root);

	return CameraTransform.GetRelativeTransform(RootOffset);
}

FVector UCharacterAnimInstance::CalculateRelativeAcceleration() const
{
	if (FVector::DotProduct(CharacterInformation.Velocity, CharacterInformation.Acceleration) > 0.f)
	{
		const float MaxAccel = CharacterOwner->GetCharacterMovement()->GetMaxAcceleration();
		return CharacterInformation.ActorRotation.UnrotateVector(CharacterInformation.Acceleration.GetClampedToMaxSize(MaxAccel) / MaxAccel);
	}

	const float MaxBrakingDec = CharacterOwner->GetCharacterMovement()->GetMaxBrakingDeceleration();
	return CharacterInformation.ActorRotation.UnrotateVector(CharacterInformation.Acceleration.GetClampedToMaxSize(MaxBrakingDec) / MaxBrakingDec);
}
#pragma endregion CalculateFunctions