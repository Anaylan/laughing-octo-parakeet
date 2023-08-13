// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/BaseAbilitySystemComponent.h"
#include "Character/BaseMovementComponent.h"
#include "Components/IKFootComponent.h"
#include "MotionWarpingComponent.h"
#include "Animation/CharacterAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Character/Abilities/BaseGameplayAbility.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Library/AbilityEnumLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/BaseWeapon.h"


const static FName NAME_WeaponRSocket = FName(TEXT("Weapon_R"));

bool ABaseCharacter::CanEquipWeapon(ABaseWeapon* WeaponToEquip)
{
	if (!WeaponToEquip || !WeaponToEquip->IsValidLowLevel())
	{
		return false;
	}
	return true;
}

void ABaseCharacter::PlayAbilityMontage_Implementation(UAnimMontage* MontageToPlay)
{
	if (!MontageToPlay) return;

	if (MontageAbilityIndex >= MontageToPlay->GetNumSections()) MontageAbilityIndex = 0;
	FName const SectionName = MontageToPlay->GetSectionName(MontageAbilityIndex);
	
	float const Duration = PlayAnimMontage(MontageToPlay, AbilityPlayRate, SectionName);
	
	if (Duration > 0.f)
	{
		MontageAbilityIndex++;
	}
	
	GetWorld()->GetTimerManager().SetTimer(MontageTimer, [this]() { MontageAbilityIndex = 0; },
		ResetATKMontageDelayOffset + MontageToPlay->GetSectionLength(MontageAbilityIndex), false);
	//GetWorld()->GetTimerManager().SetTimer(CountTimer, [this]() { AttackCount = 0; }, 
	//	Duration + 6.f, false);
}

void ABaseCharacter::Server_PlayAbilityMontage_Implementation(UAnimMontage* MontageToPlay)
{
	// PlayAnimMontage(MontageToPlay, ATKPlayRate, SectionName);
	PlayAbilityMontage(MontageToPlay);
}

bool ABaseCharacter::IsAttack()
{
	return bIsAttacking;
}

void ABaseCharacter::SetMontageFromWeaponType(TMap<EWeaponType, UAnimMontage*>& MontageMap)
{
	if (!IsValid(CurrentWeapon) || MontageMap.IsEmpty()) return;
	
	EWeaponType WType = CurrentWeapon->GetType();
	UAnimMontage** Montage = MontageMap.Find(WType);
	if (AbilityMontage == *Montage) return;
	
	if (Montage)
	{
		AbilityMontage = *Montage;
	}
	else
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("Weapon has `WeaponProperties.Type` with unknown value: %d"), static_cast<int32>(WType))
#endif
		AbilityMontage = nullptr;
	}
}

bool ABaseCharacter::CanAttack()
{
	return IsValid(CurrentWeapon) && IsAlive() && !IsAttack();
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

// Sets default values
ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseMovementComponent>(CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	
	bAbilitiesInitialized = false;
	
	bUseControllerRotationRoll = bUseControllerRotationPitch = bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 300.f, 0.f);
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 400.f;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
	MotionWarping->bSearchForWindowsInAnimsWithinMontages = true;

	AbilitySystemComponent = CreateDefaultSubobject<UBaseAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	HealthAttribute = CreateDefaultSubobject<UHealthAttributeSet>(TEXT("Health"))->GetClass();
	
	// Custom components
	IKFootComponent = CreateDefaultSubobject<UIKFootComponent>(TEXT("IK Foot Component"));
	
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -92.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesWhenNotRendered;
	GetMesh()->bEnableUpdateRateOptimizations = false;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);
	
	SetEssentialVariables();
	SetMovementModel();

	CharacterDeathNative.AddUObject(this, &ThisClass::OnDeathNative);

	// TODO: Implement character allies or NPC tag
	if (!IsLocallyControlled())
	{
		Tags.Add(TEXT("Enemy"));
	}
	
	MovementComponent->SetMovementSettings(GetTargetMovementSettings());

	// NormalAttackDelegate.AddDynamic(this, &ABaseCharacter::PlayAttack);
	// WeaponChangedDelegate.AddDynamic(this, &ABaseCharacter::ChangeWeaponMontage);
	// MontageChanged.AddDynamic(this, &ABaseCharacter::ChangeAttackMontage);
	
	SpawnWeapons();

	CharacterDeathNative.AddUObject(this, &ThisClass::OnDeathNative);
}

void ABaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	
	WeaponIndex = NULL;
	WeaponChangedDelegate.RemoveAll(this);

	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}
}

void ABaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (!IsValid(MovementComponent)) return;
	
	if (MovementComponent->MovementMode == MOVE_Flying || MovementComponent->MovementMode == MOVE_Falling)
	{
		SetLocomotionMode(ELocomotionMode::Falling);
	}
	else if (MovementComponent->MovementMode == MOVE_Walking || MovementComponent->MovementMode == MOVE_Custom ||
		MovementComponent->MovementMode == MOVE_NavWalking)
	{
		SetLocomotionMode(ELocomotionMode::Grounded);
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
	
	// SetReplicates(true);
	// SetReplicateMovement(true);
	
	MovementComponent = StaticCast<UBaseMovementComponent*>(GetCharacterMovement());
	
	// CombatComponent = StaticCast<UCombatComponent*>(GetComponentByClass<UCombatComponent>());
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
	// DOREPLIFETIME(ThisClass, HealthAttribute);
	// DOREPLIFETIME(ThisClass, AttackMontage);

	DOREPLIFETIME_CONDITION(ThisClass, AbilityMontage, ELifetimeCondition::COND_SkipOwner)
	DOREPLIFETIME_CONDITION(ThisClass, MovementComponent, ELifetimeCondition::COND_None)
	DOREPLIFETIME_CONDITION(ThisClass, Weapons, ELifetimeCondition::COND_None)
	DOREPLIFETIME_CONDITION(ThisClass, CurrentWeapon, ELifetimeCondition::COND_None)
	// DOREPLIFETIME_CONDITION(ThisClass, MotionWarping, ELifetimeCondition::COND_None);
}

float ABaseCharacter::GetHealth()
{
	if (!IsValid(HealthAttribute)) return 1.f;

	return HealthAttribute.GetDefaultObject()->GetHealth();

}

float ABaseCharacter::GetMaxHealth()
{
	if (!IsValid(HealthAttribute)) return 1.f;

	return HealthAttribute.GetDefaultObject()->GetMaxHealth();
}

float ABaseCharacter::GetDamage()
{
	if (!IsValid(HealthAttribute)) return 0.f;

	return HealthAttribute.GetDefaultObject()->GetDamage();
}

void ABaseCharacter::OnDeathNative()
{
	if (CharacterDeathDelegate.IsBound())
	{
		CharacterDeathDelegate.Broadcast();
	}
}

void ABaseCharacter::SetMovementModel()
{
	if (!IsValid(MovementTable.DataTable) || MovementTable.RowName == NAME_None) return;
	const FString ContextString = GetFullName();
	const FMovementSettings* Row = MovementTable.DataTable->FindRow<FMovementSettings>(MovementTable.RowName,
		ContextString);
	check(Row)

	MovementData = *Row;
}

void ABaseCharacter::WarpToTarget(FName WarpTargetName)
{
	// Warping settings
	TArray<AActor*> TargetPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), TargetPoints);
	
	if (MotionWarping != nullptr && TargetPoints[0] != nullptr)
	{
		MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(WarpTargetName,
			TargetPoints[0]->GetActorLocation(),
			FRotationMatrix::MakeFromX(TargetPoints[0]->GetActorLocation() - GetActorLocation()).Rotator());
	}
}

FMovementProfileSettings ABaseCharacter::GetTargetMovementSettings() const
{
	if (RotationMode == ERotationMode::Looking)
	{
		return MovementData.LookingDirection;
	}

	return MovementData.LookingDirection;
}

void ABaseCharacter::SetEssentialVariables()
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

	Speed = static_cast<float>(CurVelocity.Size2D());

	const FVector NewAcceleration = (CurVelocity - PreviousVelocity) / DeltaTime;
	Acceleration = NewAcceleration.IsNearlyZero() || IsLocallyControlled() ? NewAcceleration : Acceleration / 2;
	
	const float CurAccel = static_cast<float>(Acceleration.Size2D());
	const float MaxAccel = MovementComponent->GetMaxAcceleration();
	
	bHasMovementInput = (CurAccel / MaxAccel) > 0.f;
	bIsMoving = Speed > 0.f;
	
	AimYawRate = static_cast<float>(FMath::Abs((AimingRotation.Yaw - PreviousAimYaw) / DeltaTime));
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

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (AbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds(
			"Confirm",
			"Canceled",
			FTopLevelAssetPath(GetPathNameSafe(
				UClass::TryFindTypeSlow<UEnum>("/Script/StudyGame.EAbilityTypeInputID"))),
			static_cast<int32>(EAbilityTypeInputID::Confirm),
			static_cast<int32>(EAbilityTypeInputID::Canceled));

		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
}

void ABaseCharacter::OnRep_CurrentWeapon(ABaseWeapon* OldPrimaryWeapon)
{
	if (CurrentWeapon && CanEquipWeapon(CurrentWeapon))
	{
		// CurrentWeapon->SetActorTransform(CachedCharacter->GetMesh()->GetSocketTransform(NAME_WeaponRSocket),
		// 	false, nullptr, ETeleportType::TeleportPhysics);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			NAME_WeaponRSocket);
		
		CurrentWeapon->Equip(this);
		SetGlobalStance(EPawnGlobalStance::Armed);
		
		// MontageChanged.Broadcast();
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

	// WeaponChangedDelegate.Broadcast()
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

void ABaseCharacter::UseAbility(bool bIsPressed, const EAbilityTypeInputID AbilityTypeInputID)
{
	if (!IsValid(AbilitySystemComponent)) return;
	
	if (bIsPressed)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(AbilityTypeInputID));
	}
	else
	{
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(AbilityTypeInputID));
	}
}

#pragma region InputActions
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

void ABaseCharacter::EquipWeapon(const int32 Index)
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

void ABaseCharacter::NextWeapon()
{
	const int32 Index = Weapons.IsValidIndex(WeaponIndex + 1) ? WeaponIndex + 1 : 0;
	EquipWeapon(Index);
}

void ABaseCharacter::PrevWeapon()
{
	const int32 Index = Weapons.IsValidIndex(WeaponIndex - 1) ? WeaponIndex - 1 : Weapons.Num() - 1;
	EquipWeapon(Index);
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
		// TODO: Implement dash as initial transition to sprint
		
		SetRotationMode(ERotationMode::Looking, true);
		SetMovementProfile(EMovementProfile::Sprint);
	}
	else
	{
		SetMovementProfile(EMovementProfile::Running);
		SetRotationMode(PreviousRotationMode);
		// Broadcast delegate about change state
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

void ABaseCharacter::PlayAttackAnimation(TMap<EWeaponType, UAnimMontage*> MontageMap)
{
	if (IsLocallyControlled() && CanAttack())
	{
		UE_LOG(LogTemp, Log, TEXT("Triggered"))
		if (!MontageMap.FindKey(AbilityMontage))
		{
			SetMontageFromWeaponType(MontageMap);
		}
		
		// Called replicated function in server
		Server_PlayAbilityMontage(AbilityMontage);
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

#pragma region WeaponSystem
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
#pragma endregion WeaponSystem

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
		OnHealthChanged(Delta, EventTags);
		if (GetHealth() <= 0.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Adding DeadTag"))
			AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
		}
	}
}

void ABaseCharacter::HandlePunch_Implementation()
{
	TArray<AActor*> ActorsArray;
	TArray<FOverlapResult> OverlapResults;
	GetCurrentWeapon()->GetCollision()->GetOverlappingActors(ActorsArray, ABaseCharacter::StaticClass());
	UE_LOG(LogTemp, Error, TEXT("%hd"), ActorsArray.Num())
	int Count{};

	if (ActorsArray.Num() > 0)
	{
		for (AActor* Actor : ActorsArray)
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor name: %s"), *Actor->GetName());
			if (Actor &&
				Actor != this &&
				Actor->ActorHasTag("Enemy") &&
				UKismetSystemLibrary::DoesImplementInterface(
					Actor, UAbilitySystemInterface::StaticClass()))
			{
				// don't punch if dead
				if (Cast<IAbilitySystemInterface>(Actor)->GetAbilitySystemComponent()->HasMatchingGameplayTag(
					FGameplayTag::RequestGameplayTag("Gameplay.Status.IsDead")))
				{
					UE_LOG(LogTemp, Display, TEXT("Found IsDead"))
					continue;
				}

				const FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Weapon.Hit");
				FGameplayEventData Payload = FGameplayEventData();
				Payload.Instigator = GetInstigator();
				Payload.Target = Actor;
				Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Actor);
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), Tag, Payload);

				++Count;
			}
		}
	}

	// if our Count returns 0, it means we did not hit an enemy and we should end our ability
	if (Count == 0)
	{
		const FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Weapon.NoHit");
		FGameplayEventData Payload = FGameplayEventData();
		Payload.Instigator = GetInstigator();
		Payload.TargetData = FGameplayAbilityTargetDataHandle();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), Tag, Payload);
	}
}

void ABaseCharacter::AddStartupGameplayAbilities()
{
	check(AbilitySystemComponent)

	if (HasAuthority() && !bAbilitiesInitialized)
	{
		for (TSubclassOf<UBaseGameplayAbility> Ability: GameplayAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
				Ability, 1,
				static_cast<int32>(Ability.GetDefaultObject()->AbilityTypeID),
				this));

			UE_LOG(LogTemp, Warning, TEXT("Ability name: %s"), *Ability.GetDefaultObject()->GetName())
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
					AbilitySystemComponent);
			}
		}

		bAbilitiesInitialized = true;
	}
}
#pragma endregion AbilitySystem
