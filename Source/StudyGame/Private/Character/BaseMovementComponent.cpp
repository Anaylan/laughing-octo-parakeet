
#include "Character/BaseMovementComponent.h"

#include "Character/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

UBaseMovementComponent::UBaseMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BrakingFrictionFactor = 0.f;
	BrakingDecelerationWalking = 2.f;
	GravityScale = 1.75f;
	JumpZVelocity = 700.f;
	BrakingDecelerationFalling = 500.f;
	BrakingDecelerationWalking = 1100.f;
	MaxAcceleration = 1500.f;

	bUseSeparateBrakingFriction = true;
	
	NavAgentProps.bCanCrouch = true;
}

bool UBaseMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}

bool UBaseMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMovementMode(CMOVE_Slide);
}

bool UBaseMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void UBaseMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	BaseCharacterOwner = Cast<ABaseCharacter>(GetOwner());
}

void UBaseMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	if (MovementMode == MOVE_Walking && !bWantsToCrouch && bSafeWantsToSlide)
	{
		if (CanSlide())
		{
			SetMovementMode(MOVE_Custom, CMOVE_Slide);
		}
	}
	else if (IsCustomMovementMode(CMOVE_Slide) && !bWantsToCrouch)
	{
		SetMovementMode(MOVE_Walking);
	}
	
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
	 
}

void UBaseMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		PhysSlide(deltaTime, Iterations);
		break;
	default:
#if WITH_EDITOR
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
#endif
		break;
	}
}

void UBaseMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
                                               const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	
	if (!CharacterOwner) return;
	

	if (MovementMode == MOVE_Walking)
	{
		if (bSafeMovementSettingsUpdate)
		{
			const float UpdateMaxWalkSpeed = MovementSettings.GetSpeedForMovementProfile(SafeMovementProfile);

			MaxWalkSpeed = UpdateMaxWalkSpeed;
			bSafeMovementSettingsUpdate = false;
		}

		if (bSafeRotationModeUpdate)
		{
			bOrientRotationToMovement = SafeRotationMode == ERotationMode::Looking;
			bUseControllerDesiredRotation = SafeRotationMode == ERotationMode::Velocity;
			
			bSafeRotationModeUpdate = false;
		}
	}

	bSafeWantsToSlide = bWantsToCrouch;
}

void UBaseMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Slide) ExitSlide();

	if (IsCustomMovementMode(CMOVE_Slide))
	{
		EnterSlide(PreviousMovementMode, static_cast<ECustomMovementMode>(PreviousCustomMode));
	}
	
	if (IsFalling())
	{
		bOrientRotationToMovement = false;
	}
	else if (IsMovingOnGround())
	{
		bOrientRotationToMovement = true;
	}
}

float UBaseMovementComponent::GetMaxAcceleration() const
{
	if (IsValid(MovementSettings.GetMovementCurve()))
	{
		return MovementSettings.GetMovementCurve()->GetVectorValue(GetMappedSpeed()).X;
	}
	
	return Super::GetMaxAcceleration();
}

float UBaseMovementComponent::GetMaxBrakingDeceleration() const
{
	if (IsValid(MovementSettings.GetMovementCurve()))
	{
		return MovementSettings.GetMovementCurve()->GetVectorValue(GetMappedSpeed()).Y;
	}
	
	return Super::GetMaxBrakingDeceleration();
}

void UBaseMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	if (IsValid(MovementSettings.GetMovementCurve()))
	{
		GroundFriction = MovementSettings.GetMovementCurve()->GetVectorValue(GetMappedSpeed()).Z;;
	}
	
	Super::PhysWalking(deltaTime, Iterations);
}

float UBaseMovementComponent::GetMappedSpeed() const
{
	const float Speed = Velocity.Size2D();
	const float WalkSpeed = MovementSettings.GetSpeedForMovementProfile(EMovementProfile::Walking);
	const float RunSpeed = MovementSettings.GetSpeedForMovementProfile(EMovementProfile::Running);
	const float SprintSpeed = MovementSettings.GetSpeedForMovementProfile(EMovementProfile::Sprint);

	if (Speed > RunSpeed)
	{
		return FMath::GetMappedRangeValueClamped<float, float>({RunSpeed, SprintSpeed},
			{2.f, 3.f}, Speed);
	}

	if (Speed > WalkSpeed)
	{
		return FMath::GetMappedRangeValueClamped<float, float>({WalkSpeed, RunSpeed},
			{1.f, 2.f}, Speed);
	}

	return FMath::GetMappedRangeValueClamped<float, float>({0.f, WalkSpeed},
	{0.f, 1.f}, Speed);
}

#pragma region MovementProfile
void UBaseMovementComponent::Server_SetMovementProfile_Implementation(EMovementProfile NewMovementProfile)
{
	SafeMovementProfile = NewMovementProfile;
}

void UBaseMovementComponent::SetMovementProfile(EMovementProfile NewMovementProfile)
{
	if (NewMovementProfile != SafeMovementProfile)
	{
		if (PawnOwner->IsLocallyControlled())
		{
			SafeMovementProfile = NewMovementProfile; 
			if (GetCharacterOwner()->GetLocalRole() == ROLE_AutonomousProxy)
			{
				Server_SetMovementProfile(NewMovementProfile);
			}
			bSafeMovementSettingsUpdate = true;
			return;
		}
		if (!PawnOwner->HasAuthority())
		{
			const float UpdateMaxWalkSpeed = MovementSettings.GetSpeedForMovementProfile(SafeMovementProfile);
			MaxWalkSpeed = UpdateMaxWalkSpeed;
		}
	}
}
#pragma endregion MovementProfile

#pragma region Slide
void UBaseMovementComponent::ExitSlide()
{
	bWantsToCrouch = false;
}

void UBaseMovementComponent::EnterSlide(EMovementMode PrevMovementMode, ECustomMovementMode PrevCustomMode)
{
	bWantsToCrouch = true;
	Velocity += Velocity.GetSafeNormal2D() * SlideSettings.EnterImpulse;
	
	SetMovementMode(MOVE_Custom, CMOVE_Slide);

	FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor,
		true, nullptr);

}

bool UBaseMovementComponent::CanSlide() const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + BaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() *
		2.f * FVector::DownVector;
	const FName ProfileName = TEXT("BlockAll");
	
	bool bValidSurface = GetWorld()->LineTraceTestByProfile(Start, End, ProfileName, BaseCharacterOwner->GetIgnoreCharacterParams());
	bool bEnoughSpeed = Velocity.SizeSquared() > pow(SlideSettings.MinSpeed, 2);
	
	return bValidSurface && bEnoughSpeed;
}

void UBaseMovementComponent::PhysSlide(float DeltaTime, int32 Iterations)
{
}
#pragma endregion Slide

#pragma region Rotation
void UBaseMovementComponent::Server_SetRotationMode_Implementation(ERotationMode NewRotationMode)
{
	SafeRotationMode = NewRotationMode;
}

void UBaseMovementComponent::SetRotationMode(ERotationMode NewRotationMode)
{
	if (SafeRotationMode != NewRotationMode)
	{
		if (PawnOwner->IsLocallyControlled())
		{
			SafeRotationMode = NewRotationMode; 
			if (GetCharacterOwner()->GetLocalRole() == ROLE_AutonomousProxy)
			{
				Server_SetRotationMode(SafeRotationMode);
			}
			bSafeRotationModeUpdate = true;
			return;
		}
		if (!PawnOwner->HasAuthority())
		{
			bOrientRotationToMovement = SafeRotationMode == ERotationMode::Looking;
			bUseControllerDesiredRotation = SafeRotationMode == ERotationMode::Velocity;
		}
	}
}
#pragma endregion Rotation

#pragma region Network
bool UBaseMovementComponent::FSavedMove_Base::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,
                                                             float MaxDelta) const
{
	const FSavedMove_Base* NewCyberMove = StaticCast<FSavedMove_Base*>(NewMove.Get());
	
	if (bSavedMovementProfileUpdate != NewCyberMove->bSavedMovementProfileUpdate)
	{
		return false;
	}
	
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UBaseMovementComponent::FSavedMove_Base::Clear()
{
	Super::Clear();

	bSavedMovementProfileUpdate = false;
	bSavedRotationModeUpdate = false;
	SavedMovementProfile = EMovementProfile::Walking;
	SavedRotationMode = ERotationMode::Looking;
}

uint8 UBaseMovementComponent::FSavedMove_Base::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bSavedMovementProfileUpdate)
	{
		Result |= FLAG_Custom_0;
	}
	
	return Result;
}

void UBaseMovementComponent::FSavedMove_Base::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
	FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	const UBaseMovementComponent* CharacterMovement = Cast<UBaseMovementComponent>(C->GetCharacterMovement());
	
	if (CharacterMovement)
	{
		bSavedMovementProfileUpdate = CharacterMovement->bSafeMovementSettingsUpdate;
		bSavedRotationModeUpdate = CharacterMovement->bSafeRotationModeUpdate;
		bSavedWantsToSlide = CharacterMovement->bSafeWantsToSlide;
		SavedMovementProfile = CharacterMovement->SafeMovementProfile;
		SavedRotationMode = CharacterMovement->SafeRotationMode;
	}
}

void UBaseMovementComponent::FSavedMove_Base::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);
	
	UBaseMovementComponent* CharacterMovement = Cast<UBaseMovementComponent>(C->GetCharacterMovement());
	
	CharacterMovement->SafeMovementProfile = SavedMovementProfile;
	CharacterMovement->bSafeWantsToSlide = bSavedWantsToSlide;
	
}

UBaseMovementComponent::FNetworkPredictionData_Client_Base::FNetworkPredictionData_Client_Base(
	const UCharacterMovementComponent& ClientMovement)
		: Super(ClientMovement)
{
}

FSavedMovePtr UBaseMovementComponent::FNetworkPredictionData_Client_Base::AllocateNewMove()
{
	return MakeShared<FSavedMove_Base>();
}

FNetworkPredictionData_Client* UBaseMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr);

	if (!ClientPredictionData)
	{
		UBaseMovementComponent* MutableThis = const_cast<UBaseMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Base(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}
	
	return ClientPredictionData;
}

void UBaseMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bSafeMovementSettingsUpdate = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}
#pragma endregion Network