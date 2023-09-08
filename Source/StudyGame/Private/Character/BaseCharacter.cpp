// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "KismetAnimationLibrary.h"
#include "Components/BaseAbilitySystemComponent.h"
#include "Character/BaseMovementComponent.h"
#include "Components/IKFootComponent.h"
#include "Animation/CharacterAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Abilities/BaseGameplayAbility.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Library/AbilityEnumLibrary.h"
#include "Library/MathBlueprintFunctionLibrary.h"
#include "Library/StructEnumLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/BaseWeapon.h"

// Sets default values
ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseMovementComponent>(CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	
	bAbilitiesInitialized = false;
	
	bUseControllerRotationRoll = bUseControllerRotationPitch = bUseControllerRotationYaw = false;
	bClientCheckEncroachmentOnNetUpdate = true;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 300.f, 0.f);
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 400.f;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

	AbilitySystemComponent = CreateDefaultSubobject<UBaseAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	HealthAttribute = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("Stats"));
	
	// Custom components
	IKFootComponent = CreateDefaultSubobject<UIKFootComponent>(TEXT("IK Foot Component"));
	
	GetMesh()->SetRelativeLocation_Direct({0.f, 0.f, -92.f});
	GetMesh()->SetRelativeRotation_Direct({0.f, -90.f, 0.f});
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesWhenNotRendered;
	GetMesh()->bEnableUpdateRateOptimizations = false;
}

bool ABaseCharacter::CanEquipWeapon(ABaseWeapon* WeaponToEquip)
{
	if (!WeaponToEquip || !WeaponToEquip->IsValidLowLevel())
	{
		return false;
	}

	// We can't equip weapon in battle
	if (!IsAlive() || IsInCombat())
	{
		return false;
	}
	
	return true;
}

void ABaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
}

void ABaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
}

void ABaseCharacter::PlayAbilityMontage_Implementation(UAnimMontage* MontageToPlay)
{
	if (!MontageToPlay) return;
	
	if (MontageAbilityIndex >= MontageToPlay->GetNumSections()) MontageAbilityIndex = 0;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance->IsValidLowLevelFast()) return;
	
	FName const SectionName = MontageToPlay->GetSectionName(MontageAbilityIndex);
	
	float const Duration = AnimInstance->Montage_Play(MontageToPlay, AbilityPlayRate);

	if (Duration > 0.f)
	{
		AnimInstance->Montage_JumpToSection(SectionName, MontageToPlay);
		MontageAbilityIndex++;
		GetWorld()->GetTimerManager().SetTimer(MontageTimer, [this]() { MontageAbilityIndex = 0; },
			ResetATKMontageDelay + MontageToPlay->GetSectionLength(MontageAbilityIndex), false);
	}
	
	MontageEndedDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

	BlendingOutDelegate.BindUObject(this, &ThisClass::OnMontageBlendingOut);
	AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);
}

void ABaseCharacter::Server_PlayAbilityMontage_Implementation(UAnimMontage* MontageToPlay)
{
	PlayAbilityMontage(MontageToPlay);
}

bool ABaseCharacter::IsInCombat() const
{
	// TODO: Add "in battle state"
	
	return false;
}

UBaseMovementComponent* ABaseCharacter::GetCustomMovementComponent() const
{
	return StaticCast<UBaseMovementComponent*>(GetCharacterMovement());
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return Cast<UAbilitySystemComponent>(AbilitySystemComponent.Get());
}

bool ABaseCharacter::IsPlayingAnimation() const
{
	return bIsPlayingAnimation;
}

bool ABaseCharacter::CanAttack() const
{
	return IsValid(CurrentWeapon) && IsAlive() && !IsPlayingAnimation();
}

void ABaseCharacter::OnMontageEnded_Implementation(class UAnimMontage* Montage, bool bInterrupted)
{
	if (!IsDataAbilitySend)
	{
		FGameplayEventData Payload = FGameplayEventData();
		Payload.Instigator = GetInstigator();
		Payload.TargetData = FGameplayAbilityTargetDataHandle();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), WeaponNoHitTag, Payload);
	}
	if (IsDataAbilitySend) IsDataAbilitySend = false;
}

void ABaseCharacter::OnMontageBlendingOut_Implementation(class UAnimMontage* Montage, bool bInterrupted)
{
	if (!IsDataAbilitySend)
	{
		FGameplayEventData Payload = FGameplayEventData();
		Payload.Instigator = GetInstigator();
		Payload.TargetData = FGameplayAbilityTargetDataHandle();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), WeaponNoHitTag, Payload);
	}
	if (IsDataAbilitySend) IsDataAbilitySend = false;
}

void ABaseCharacter::Server_SetCurrentWeapon_Implementation(ABaseWeapon* NewWeapon)
{
	ABaseWeapon* OldPrimaryWeapon = CurrentWeapon;
	CurrentWeapon = NewWeapon;
	OnRep_CurrentWeapon(OldPrimaryWeapon);
}

void ABaseCharacter::SpawnWeapons()
{
	if (HasAuthority())
	{
		for (const TSubclassOf<ABaseWeapon>& WeaponClass: DefaultWeapons)
		{
			if (!WeaponClass) continue;
			
			FActorSpawnParameters Params;
			Params.Owner = this;
			
			ABaseWeapon* SpawnedWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClass, Params);
			const int32 Index = Weapons.Add(SpawnedWeapon);
			SpawnedWeapon->UnEquip();
			
			if (Index == WeaponIndex)
			{
				CurrentWeapon = SpawnedWeapon;
				OnRep_CurrentWeapon(nullptr);
			}
		}
	}
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	if ((!WeaponHitTag.IsValid() || !WeaponNoHitTag.IsValid() || !IsDeadTag.IsValid()) && WITH_EDITOR)
	{
		GEngine->AddOnScreenDebugMessage(NULL, 10.f, FColor::Red, TEXT("Tag/tags for ability system component is empty, please set value for tag/tags"));
	}
	Super::BeginPlay();

	SetReplicateMovement(true);
	
	InitMovementStates();
	InitMovementModel();

	// TODO: Implement character allies or NPC tag
	if (!IsLocallyControlled())
	{
		Tags.Add(TEXT("Enemy"));
	}
	
	MovementComponent->SetMovementSettings(GetTargetMovementSettings());
	
	SpawnWeapons();

	CharacterDeathNative.AddUObject(this, &ThisClass::OnDeathNative);
}

void ABaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	
	WeaponIndex = NULL;
	
	for (ABaseWeapon* WeaponClass: Weapons)
	{
		if (!WeaponClass) continue;
		WeaponClass->MarkAsGarbage();
	}

	if (CurrentWeapon) CurrentWeapon->MarkAsGarbage();
}

void ABaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (!MovementComponent.IsValid()) return;
	
	if (MovementComponent->IsFalling())
	{
		SetLocomotionMode(ELocomotionMode::Falling);
	}
	else if (MovementComponent->IsMovingOnGround())
	{
		SetLocomotionMode(ELocomotionMode::Grounded);
	}
	else if (MovementComponent->IsSwimming())
	{
		SetLocomotionMode(ELocomotionMode::Swimming);
	}
}

bool ABaseCharacter::CanUpdateMovementRotation()
{
	return (bIsMoving && bHasMovementInput || Speed > 150.f) && !HasAnyRootMotion();
}

void ABaseCharacter::UpdateGroundedRotation(float DeltaTime)
{
	if (CanUpdateMovementRotation())
	{
		
	}
	else
	{
		if (float const RotationAmount = GetAnimCurveValue(UCharacterAnimInstance::NAME_RotationAmount); FMath::Abs(RotationAmount) > .001f)
		{
			if (GetLocalRole() == ROLE_AutonomousProxy)
			{
			}
			else
			{
				AddActorWorldRotation({0.f, RotationAmount * (DeltaTime / (1.0f / 30.0f)), 0.f});
			}
		}
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateEssentialVariables(DeltaTime);
	UpdateGroundedRotation(DeltaTime);
	
	PreviousVelocity = GetVelocity();
	PreviousAcceleration = MovementComponent->GetCurrentAcceleration();
	PreviousAimYaw = static_cast<float>(GetAimingRotation().Yaw);
}

void ABaseCharacter::OnJumped_Implementation()
{
	if (IsLocallyControlled() && JumpedDelegate.IsBound())
	{
		JumpedDelegate.Broadcast();
	}
}

void ABaseCharacter::Server_SetOverlayState_Implementation(EOverlayState NewOverlayState)
{
	SetOverlayState(NewOverlayState);
}

void ABaseCharacter::Server_SetLocomotionMode_Implementation(ELocomotionMode NewLocomotionMode)
{
	SetLocomotionMode(NewLocomotionMode);
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetMesh()->AddTickPrerequisiteActor(this);
	
	MovementComponent = StaticCast<UBaseMovementComponent*>(GetCharacterMovement());
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MovementProfile)
	DOREPLIFETIME(ThisClass, RotationMode)
	DOREPLIFETIME(ThisClass, LocomotionMode)
	DOREPLIFETIME(ThisClass, MovementAction)
	DOREPLIFETIME(ThisClass, GlobalStance)
	DOREPLIFETIME(ThisClass, OverlayState)
	DOREPLIFETIME(ThisClass, WeaponIndex)
	DOREPLIFETIME(ThisClass, MontageAbilityIndex)
	DOREPLIFETIME(ThisClass, IsDataAbilitySend)
	DOREPLIFETIME(ThisClass, bIsPlayingAnimation)

	DOREPLIFETIME_CONDITION(ThisClass, AbilityMontage, ELifetimeCondition::COND_SkipOwner)
	DOREPLIFETIME_CONDITION(ThisClass, MovementComponent, ELifetimeCondition::COND_None)
	DOREPLIFETIME_CONDITION(ThisClass, Weapons, ELifetimeCondition::COND_None)
	DOREPLIFETIME_CONDITION(ThisClass, CurrentWeapon, ELifetimeCondition::COND_None)
}

float ABaseCharacter::GetHealth()
{
	return HealthAttribute ? HealthAttribute->GetHealth() : 1.f;
}

float ABaseCharacter::GetMaxHealth()
{
	return HealthAttribute ? HealthAttribute->GetMaxHealth() : 1.f;
}

float ABaseCharacter::GetDamage()
{
	return HealthAttribute ? HealthAttribute->GetDamage() : 0.f;
}

void ABaseCharacter::Server_SetIsPlayingAnimation_Implementation(bool bPlayingAnimation)
{
	SetIsPlayingAnimation(bPlayingAnimation);
}

void ABaseCharacter::SetIsPlayingAnimation(bool bPlayingAnimation)
{
	if (bIsPlayingAnimation != bPlayingAnimation)
	{
		bIsPlayingAnimation = bPlayingAnimation;

		if (HasAuthority())
		{
			Server_SetIsPlayingAnimation(bPlayingAnimation);
		}
	}
}

void ABaseCharacter::OnDeathNative()
{
	if (CharacterDeathDelegate.IsBound())
	{
		CharacterDeathDelegate.Broadcast();
	}
}

void ABaseCharacter::InitMovementModel()
{
	if (!IsValid(MovementTable.DataTable) || MovementTable.RowName.IsNone()) return;
	const FString ContextString = GetFullName();
	const FMovementSettings* Row = MovementTable.DataTable->FindRow<FMovementSettings>(MovementTable.RowName,
		ContextString);
	check(Row)

	MovementData = *Row;
}

FMovementProfileSettings ABaseCharacter::GetTargetMovementSettings() const
{
	if (RotationMode == ERotationMode::Looking)
	{
		return MovementData.LookingDirection;
	}
	if (RotationMode == ERotationMode::Velocity)
	{
		return MovementData.VelocityDirection;
	}

	return MovementData.LookingDirection;
}

UAnimMontage* ABaseCharacter::GetMontageFromWeaponMap(TMap<EWeaponType, FMontageGlobalState> MontageMap)
{
	if (!IsValid(CurrentWeapon) || MontageMap.IsEmpty()) return nullptr;
	
	EWeaponType WType = CurrentWeapon->GetType();
	if (const FMontageGlobalState* Montage = MontageMap.Find(WType))
	{
		if (MovementComponent->IsFalling() && Montage->InAir) return Montage->InAir;
		if (MovementComponent->IsMovingOnGround() && Montage->OnGround) return Montage->OnGround;
	}
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("Weapon has `WeaponProperties.Type` with unknown value: %d, in: %s"), static_cast<int32>(WType),
		*UStructEnumLibrary::EnumToString<EMovementMode>(MovementComponent->MovementMode))
#endif
	return nullptr;
}

UAnimMontage* ABaseCharacter::GetMontageByDirection(const FMontageDirection MontageDirection)
{
	const EMovementDirection Dir = UMathBlueprintFunctionLibrary::CalculateMovementDirection(70.f, 130.f,
		-70.f, -130.f, UKismetAnimationLibrary::CalculateDirection(GetVelocity(), GetActorRotation()));
	switch (Dir)
	{
	case EMovementDirection::Right:
		return MontageDirection.Right.LoadSynchronous();
	case EMovementDirection::Backward:
		return MontageDirection.Left.LoadSynchronous();
	case EMovementDirection::Left:
		return MontageDirection.Backward.LoadSynchronous();
	default:
		return MontageDirection.Forward.LoadSynchronous();
	}
}

void ABaseCharacter::InitMovementStates()
{
	SetMovementProfile(MovementProfile, true);
	SetRotationMode(RotationMode, true);
	SetGlobalStance(GlobalStance, true);
	SetOverlayState(OverlayState, true);
}

void ABaseCharacter::UpdateEssentialVariables(float DeltaTime)
{
	AimingRotation = GetControlRotation();

	const FVector CurVelocity = GetVelocity();

	Speed = UE_REAL_TO_FLOAT(CurVelocity.Size2D());

	const FVector NewAcceleration = (CurVelocity - PreviousVelocity) / DeltaTime;
	Acceleration = NewAcceleration.IsNearlyZero() || IsLocallyControlled() ? NewAcceleration : Acceleration;
	
	const float CurAccel = UE_REAL_TO_FLOAT(Acceleration.Size2D());
	const float MaxAccel = MovementComponent->GetMaxAcceleration();
	
	bHasMovementInput = (CurAccel / MaxAccel) > 0.f;
	bIsMoving = Speed > 0.f;
	
	AimYawRate = UE_REAL_TO_FLOAT(FMath::Abs((AimingRotation.Yaw - PreviousAimYaw) / DeltaTime));
}

FCollisionQueryParams ABaseCharacter::GetIgnoreCharacterParams() const
{
	FCollisionQueryParams Params;
	TArray<AActor*> ChildrenActors;
	GetAllChildActors(ChildrenActors);
	Params.AddIgnoredActors(ChildrenActors);
	Params.AddIgnoredActor(this);	
	
	return Params;
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();
	}
}

void ABaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		
		if (InputComponent)
		{
			const FGameplayAbilityInputBinds Binds(
				"Confirm",
				"Canceled",
				FTopLevelAssetPath(GetPathNameSafe(
					UClass::TryFindTypeSlow<UEnum>("/Script/StudyGame.EAbilityTypeInputID"))),
				StaticCast<int32>(EAbilityTypeInputID::Confirm),
				StaticCast<int32>(EAbilityTypeInputID::Canceled));

			AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
		}
	}
}

void ABaseCharacter::OnRep_CurrentWeapon(ABaseWeapon* OldPrimaryWeapon)
{
	if (CurrentWeapon && CanEquipWeapon(CurrentWeapon))
	{
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			CurrentWeapon->GetTargetSocketName());
		
		CurrentWeapon->Equip(this);
		SetGlobalStance(EPawnGlobalStance::Armed);
	}
	else
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->UnEquip();
		CurrentWeapon = nullptr;
		SetGlobalStance(EPawnGlobalStance::Unarmed);
	}

	if (OldPrimaryWeapon)
	{
		OldPrimaryWeapon->UnEquip();
		OldPrimaryWeapon = nullptr;
	}
	
	WeaponChangedDelegate.Broadcast(CurrentWeapon, OldPrimaryWeapon);
}

void ABaseCharacter::EnableRagdoll()
{
	USkeletalMeshComponent* MeshComp = StaticCast<USkeletalMeshComponent*>(GetMesh());
	// Ragdoll
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetAllBodiesSimulatePhysics(true);
	MeshComp->WakeAllRigidBodies();
	MeshComp->bBlendPhysics = true;
	MovementComponent->StopMovementKeepPathing();	
}

float ABaseCharacter::GetAnimCurveValue(const FName CurveName)
{
	if (GetMesh()->GetAnimInstance())
	{
		return GetMesh()->GetAnimInstance()->GetCurveValue(CurveName);
	}
	return 0.0f;
}

void ABaseCharacter::DisableGravity()
{
	MovementComponent->GravityScale = 0.f;
	MovementComponent->StopMovementKeepPathing();
	// TODO: Remove root motion add LaunchCharacter() or AddMovementInput(), it's allow use this ability with sprint or long dash
}

void ABaseCharacter::EnableGravity(float Gravity)
{
	MovementComponent->GravityScale = Gravity;
}

#pragma region InputActions
void ABaseCharacter::UseAbility(bool bIsPressed, const EAbilityTypeInputID AbilityTypeInputID)
{
	if (!AbilitySystemComponent) return;
	
	if (bIsPressed)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<uint8>(AbilityTypeInputID));
	}
	else
	{
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<uint8>(AbilityTypeInputID));
	}
}

void ABaseCharacter::JumpAction(bool bValue)
{	
	if (bValue)
	{		
		Jump();
	}
	else
	{
		StopJumping();
	}
}

void ABaseCharacter::EquipWeaponAction(const int32 Index)
{
	if (!Weapons.IsValidIndex(Index) || CurrentWeapon == Weapons[Index]) return;
	
	if (IsLocallyControlled())
	{
		WeaponIndex = Index;
		ABaseWeapon* OldWeapon = CurrentWeapon;
		CurrentWeapon = Weapons[Index];
		OnRep_CurrentWeapon(OldWeapon);
	}
	else if (!HasAuthority())
	{
		Server_SetCurrentWeapon(Weapons[Index]);
	}
}

void ABaseCharacter::NextWeaponAction()
{
	const int32 Index = Weapons.IsValidIndex(WeaponIndex + 1) ? WeaponIndex + 1 : 0;
	EquipWeaponAction(Index);
}

void ABaseCharacter::PrevWeaponAction()
{
	const int32 Index = Weapons.IsValidIndex(WeaponIndex - 1) ? WeaponIndex - 1 : Weapons.Num() - 1;
	EquipWeaponAction(Index);
}

void ABaseCharacter::MoveAction(FVector2D Value)
{
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);
	
	const FVector ForwardDirectionDirection =  FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirectionDirection =  FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	AddMovementInput(ForwardDirectionDirection, Value.X);
	AddMovementInput(RightDirectionDirection, Value.Y);
}

void ABaseCharacter::LookAction(FVector2D Value)
{
	AddControllerYawInput(SensitivityX * Value.X);
	AddControllerPitchInput(SensitivityY * -1 * Value.Y);
}

void ABaseCharacter::SprintAction()
{
	if (MovementProfile != EMovementProfile::Sprint)
	{
		SetRotationMode(ERotationMode::Looking);
		SetMovementProfile(EMovementProfile::Sprint);
	}
	else
	{
		SetMovementProfile(EMovementProfile::Running);
		SetRotationMode(PreviousRotationMode);
	}
}

void ABaseCharacter::AimAction()
{
	if (MovementProfile != EMovementProfile::Sprint)
	{
		if (RotationMode != ERotationMode::Looking)
		{
			SetRotationMode(ERotationMode::Looking);
		}
		else
		{
			SetRotationMode(ERotationMode::Velocity);
		}
	}
}

void ABaseCharacter::PlayAttackAnimation(const TMap<EWeaponType, FMontageGlobalState> MontageMap)
{
	if (IsLocallyControlled())
	{
		AbilityMontage = GetMontageFromWeaponMap(MontageMap);
		
		// Called replicated function in server
		Server_PlayAbilityMontage(AbilityMontage.LoadSynchronous());
	}
}

void ABaseCharacter::WalkAction()
{
	if (MovementProfile != EMovementProfile::Walking)
	{
		SetMovementProfile(EMovementProfile::Walking);
	}
	else
	{
		SetMovementProfile(EMovementProfile::Running);
	}
}
#pragma endregion InputActions

#pragma region MovementProfile
void ABaseCharacter::SetMovementProfile(EMovementProfile NewMovementProfile, bool bForce)
{
	if (MovementProfile != NewMovementProfile || bForce)
	{
		PreviousMovementProfile = MovementProfile;
		
		MovementProfile = NewMovementProfile;
		MovementComponent->SetMovementProfile(MovementProfile);
		
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_SetMovementProfile(NewMovementProfile);
		}
	}
}

void ABaseCharacter::Server_SetMovementProfile_Implementation(EMovementProfile NewMovementProfile)
{
	SetMovementProfile(NewMovementProfile);
}
#pragma endregion MovementProfile

#pragma region RotationMode
void ABaseCharacter::SetRotationMode(ERotationMode NewRotationMode, bool bForce)
{
	if (RotationMode != NewRotationMode || bForce)
	{
		// Set previous rotation mode
		PreviousRotationMode = RotationMode;
		
		RotationMode = NewRotationMode;
		MovementComponent->SetRotationMode(RotationMode);
		
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_SetRotationMode(NewRotationMode);
		}
	}
}

void ABaseCharacter::SetLocomotionMode(ELocomotionMode NewLocomotionMode, bool bForce)
{
	if (NewLocomotionMode != LocomotionMode || bForce)
	{
		LocomotionMode = NewLocomotionMode;

		if (HasAuthority())
		{
			Server_SetLocomotionMode(NewLocomotionMode);
		}
	}
}

void ABaseCharacter::SetOverlayState(EOverlayState NewOverlayState, bool bForce)
{
	if (NewOverlayState != OverlayState || bForce)
	{
		OverlayState = NewOverlayState;

		if (HasAuthority())
		{
			Server_SetOverlayState(NewOverlayState);
		}
	}
}

void ABaseCharacter::Server_SetRotationMode_Implementation(ERotationMode NewRotationMode)
{
	SetRotationMode(NewRotationMode);
}
#pragma endregion RotationMode

#pragma region GlobalStance
void ABaseCharacter::SetGlobalStance(EPawnGlobalStance NewGlobalStance, bool bForce)
{
	if (NewGlobalStance != GlobalStance)
	{
		GlobalStance = NewGlobalStance;

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_SetGlobalStance(NewGlobalStance);
		}
	}
}

void ABaseCharacter::Server_SetGlobalStance_Implementation(EPawnGlobalStance NewGlobalStance)
{
	SetGlobalStance(NewGlobalStance);
}
#pragma endregion GlobalStance

#pragma region AbilitySystem
void ABaseCharacter::HandleDamage(float InDamage, const FHitResult& HitResult, const FGameplayTagContainer& DamageTags,
	ABaseCharacter* InstigatorCharacter, AActor* DamageCauser)
{
	OnDamaged(InDamage, HitResult, DamageTags, InstigatorCharacter, DamageCauser);
}


void ABaseCharacter::HandleHealthChanged(float Delta, const FGameplayTagContainer& EventTags)
{
	// We only call the BP callback if this is not the initial ability setup
	if (bAbilitiesInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("Delta: %f"), Delta)
		UE_LOG(LogTemp, Warning, TEXT("Health: %f"), GetHealth())
		OnHealthChanged(Delta, EventTags);
		if (GetHealth() <= 0.f)
		{
#if WITH_EDITOR
			UE_LOG(LogTemp, Warning, TEXT("Adding IsDeadTag"))
#endif
			AbilitySystemComponent->AddLooseGameplayTag(IsDeadTag);
		}
	}
}

void ABaseCharacter::HandlePunch_Implementation()
{
	TArray<FHitResult> OutHits;
	UCapsuleComponent* CachedCollision = GetCurrentWeapon()->GetCollision();
	FVector Start = CachedCollision->GetComponentLocation() -
		CachedCollision->GetComponentRotation().UnrotateVector({0.f, 0.f, 
		CachedCollision->GetScaledCapsuleHalfHeight()});
	
	FVector End = CachedCollision->GetComponentLocation() +
		CachedCollision->GetComponentRotation().UnrotateVector({0.f, 0.f, 
		CachedCollision->GetScaledCapsuleHalfHeight()});

	FCollisionShape CollisionShape;
	CollisionShape.SetCapsule(CachedCollision->GetScaledCapsuleRadius() + .5f,
							CachedCollision->GetScaledCapsuleHalfHeight() + .5f);
	
	if (GetWorld()->SweepMultiByChannel(OutHits, Start, End, FQuat::Identity,
		ECC_WorldDynamic, CollisionShape))
	{
		for (FHitResult HitResult : OutHits)
		{
			if (AActor* Actor = HitResult.GetActor(); Actor &&
				Actor != this &&
				(Actor->ActorHasTag("Enemy") || Actor->ActorHasTag("Destroyable")) &&
				!Actor->IsA(ABaseWeapon::StaticClass()))
			{
				DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 7.f, 3, FColor::Green, false, 10.f);
				// don't punch if dead
				if (Cast<IAbilitySystemInterface>(Actor)->GetAbilitySystemComponent()->HasMatchingGameplayTag(
					IsDeadTag))
				{
					continue;
				}
			
				FGameplayEventData Payload = FGameplayEventData();
				Payload.Instigator = GetInstigator();
				Payload.Target = Actor;
				Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Actor);
			
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), WeaponHitTag, Payload);

				if (!IsDataAbilitySend) IsDataAbilitySend = true;
			}
		}
	}
}

void ABaseCharacter::AddStartupGameplayAbilities()
{
	check(AbilitySystemComponent.Get())

	if (HasAuthority() && !bAbilitiesInitialized)
	{
		for (TPair<EAbilityTypeInputID, TSubclassOf<UBaseGameplayAbility>> Ability: StandardAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
				Ability.Value, 1,
				static_cast<int32>(Ability.Key),
				this));
		}

		for (const TSubclassOf<UGameplayEffect> Effect: PassiveGameplayEffects)
		{
			FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
			EffectContextHandle.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(
				Effect, 1, EffectContextHandle);

			if (NewHandle.IsValid())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(),
					AbilitySystemComponent.Get());
			}
		}

		bAbilitiesInitialized = true;
	}
}

void ABaseCharacter::Jump()
{
	Super::Jump();

	StopAnimMontage();
}
#pragma endregion AbilitySystem
