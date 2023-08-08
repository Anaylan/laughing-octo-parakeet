// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Library/CharacterEnumLibrary.h"
#include "BaseMovementComponent.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "Attributes/HealthAttributeSet.h"
#include "Library/AbilityEnumLibrary.h"
#include "BaseCharacter.generated.h"

class ABaseWeapon;
class UBaseAbilitySystemComponent;
class UBaseGameplayAbility;
class UCameraComponent;
class USpringArmComponent;
class UIKFootComponent;
class UMotionWarpingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumpedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMontageChangedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponChangedDelegate, ABaseWeapon*, Weapon, const ABaseWeapon*, OldWeapon);

/** Delegate called when a character dies */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeath);
DECLARE_MULTICAST_DELEGATE(FOnCharacterDeathNative);

UCLASS(Abstract)
class STUDYGAME_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:

	friend UHealthAttributeSet;

	FGameplayTag DeadTag;

	int16 AttackCount = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAttacking = false;
	UPROPERTY(BlueprintReadOnly, Category = "Config")
	uint8 bAttackGate:1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Configuration", meta = (UIMin = .5f, ClampMin = .5f))
	float ATKPlayRate = 1.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Montage Settings", meta = (UIMin = .0f, ClampMin = .0f))
	float ResetATKMontageDelayOffset = 0.2f;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category = "State")
	// Buffered Weapons
	TArray<TObjectPtr<ABaseWeapon>> Weapons;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon, Category = "State")
	TObjectPtr<ABaseWeapon> CurrentWeapon = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;
	
	int32 MontageATKIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "State")
	int32 WeaponIndex = 0;

	virtual bool CanEquipWeapon(ABaseWeapon* WeaponToEquip);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void PlayAttackMontage(UAnimMontage* MontageToPlay);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_PlayAttackMontage(UAnimMontage* MontageToPlay);
	
	UFUNCTION(Server, Reliable)
	void Server_SetCurrentWeapon(ABaseWeapon* NewWeapon);

	UPROPERTY(EditDefaultsOnly, Category = "Configurations")
	TArray<TSubclassOf<ABaseWeapon>> DefaultWeapons;

	FTimerHandle AttackTimer;
	FTimerHandle CountTimer;
	FTimerHandle MontageTimer;

	UFUNCTION(BlueprintCallable)
	void SetMontageFromMap_Implemented(TMap<EWeaponType, UAnimMontage*>& MontageMap);


	bool CanAttack();
	
	/***************
	* References
	**************/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input | Camera")
	TObjectPtr<UCameraComponent> FollowCamera = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input | Camera")
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	TObjectPtr<UIKFootComponent> IKFootComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMotionWarpingComponent> MotionWarping = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Settings", meta = (RowType = "/Script/StudyGame.MovementSettings"))
	FDataTableRowHandle MovementTable;
	UPROPERTY(Replicated)
	TObjectPtr<UBaseMovementComponent> MovementComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBaseAbilitySystemComponent> AbilitySystemComponent = nullptr;
	
	/***************
	* Ability system
	**************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UBaseGameplayAbility>> GameplayAbilities;
	UPROPERTY()
	uint8 bAbilitiesInitialized:1;

	/**
	 * Called wge character takes damage.
	 * 
	 * @param InDamage		How much damage to apply.
	 * @param HitResult		The hit info that generated this damage.
	 * @param DamageTags	The gameplay tags of the event that did the damage.
	 * @param InstigatorCharacter		The character that initiated this damage.
	 * @param DamageCauser		The Actor that directly caused the damage (e.g. the projectile that exploded, the rock that landed on you)
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnDamaged(float InDamage, const FHitResult& HitResult, const FGameplayTagContainer& DamageTags,
		ABaseCharacter* InstigatorCharacter, AActor* DamageCauser);

	/**
	 * Called wge character takes damage.
	 * 
	 * @param Delta		How much health is changed.
	 * @param EventTags		The gameplay tags of the event that changed health.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float Delta, const FGameplayTagContainer& EventTags);
	
	/***************
	* Configuration
	**************/
	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Movement Settings")
	EMovementProfile MovementProfile = EMovementProfile::Running;
	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Movement Settings")
	ERotationMode RotationMode = ERotationMode::Looking;
	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Movement Settings")
	EMovementAction MovementAction = EMovementAction::None;
	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Movement Settings")
	EPawnGlobalStance GlobalStance = EPawnGlobalStance::Unarmed;
	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Movement Settings")
	ELocomotionMode LocomotionMode = ELocomotionMode::Grounded;
	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Movement Settings")
	EOverlayState OverlayState = EOverlayState::Katana;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTag WeaponHitTag;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTag WeaponNoHitTag;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTag IsDeadTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input | Camera")
	float SensitivityX = 1.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input | Camera")
	float SensitivityY = 1.f;
	
	/********************
	* Working variables
	*******************/
	UPROPERTY(BlueprintReadOnly)
	float Speed = 0.f;
	UPROPERTY(BlueprintReadOnly)
	float AimYawRate = 0.f;
	UPROPERTY(BlueprintReadOnly)
	bool bHasMovementInput = false;
	UPROPERTY(BlueprintReadOnly)
	bool bIsMoving = false;
	UPROPERTY(BlueprintReadOnly)
	bool bIsAlive = true;
	UPROPERTY(BlueprintReadOnly)
	FRotator AimingRotation = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadOnly)
	FRotator CurrentRotation = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadOnly)
	FVector Acceleration = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly)
	FMovementProfileSettings MovementProfileSettings;
	UPROPERTY(BlueprintReadOnly)
	FMovementSettings MovementData;
	
	/*****************
	* Previous tick
	*****************/
	UPROPERTY(BlueprintReadOnly)
	float PreviousAimYaw = 0.f;
	UPROPERTY(BlueprintReadOnly)
	FVector PreviousVelocity = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly)
	FVector PreviousAcceleration = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Category = "Movement Settings")
	ERotationMode PreviousRotationMode = ERotationMode::Looking;
	UPROPERTY(BlueprintReadOnly, Category = "Movement Settings")
	EMovementProfile PreviousMovementProfile = EMovementProfile::Walking;

	UFUNCTION()
	virtual void SpawnWeapons();
	
public:
	// Sets default values for this character's properties
	explicit ABaseCharacter(const FObjectInitializer& ObjectInitializer);

	FCollisionQueryParams GetIgnoreCharacterParams() const;

	virtual void HandleDamage(float InDamage, const FHitResult& HitResult, const FGameplayTagContainer& DamageTags,
		ABaseCharacter* InstigatorCharacter, AActor* DamageCauser);
	
	virtual void HandleHealthChanged(float Delta, const FGameplayTagContainer& EventTags);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	virtual void HandlePunch();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	UFUNCTION()
	void OnRep_CurrentWeapon(ABaseWeapon* OldPrimaryWeapon);
	
protected:
	virtual void EnableRagdoll();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	
	virtual bool CanUpdateMovementRotation();
	void UpdateGroundedRotation(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UHealthAttributeSet> HealthAttribute = nullptr;

	void AddStartupGameplayAbilities();

public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float GetAnimCurveValue(FName CurveName);

	/********************
	* Keyboard actions
	*******************/
	UFUNCTION(BlueprintCallable)
	void UseAbility(bool bIsPressed, const EAbilityTypeInputID AbilityTypeInputID);
	UFUNCTION(BlueprintCallable)
	void MoveAction(FVector2D Value);
	UFUNCTION(BlueprintCallable)
	void LookAction(FVector2D Value);
	UFUNCTION(BlueprintCallable)
	void WalkAction();
	UFUNCTION(BlueprintCallable)
	void JumpAction(bool bValue);
	UFUNCTION(BlueprintCallable)
	virtual void EquipWeapon(const int32 Index);
	UFUNCTION(BlueprintCallable)
	virtual void NextWeapon();
	UFUNCTION(BlueprintCallable)
	virtual void PrevWeapon();
	// Maybe, this function is not need to be override
	UFUNCTION(BlueprintCallable)
	virtual void SprintAction();
	UFUNCTION(BlueprintCallable)
	void AimAction();
	UFUNCTION(BlueprintCallable)
	void PlayAttackAnimation(TMap<EWeaponType, UAnimMontage*> MontageMap);

	void SetMovementProfile(EMovementProfile NewMovementProfile, bool bForce = false);
	void SetGlobalStance(EPawnGlobalStance NewGlobalStance, bool bForce = false);
	void SetRotationMode(ERotationMode NewRotationMode, bool bForce = false);
	void SetLocomotionMode(ELocomotionMode NewLocomotionMode, bool bForce = false);
	void SetOverlayState(EOverlayState NewOverlayState, bool bForce = false);
	
	virtual void OnJumped_Implementation() override;

	/*******************
	* Server functions
	******************/
	UFUNCTION(Server, Reliable)
	void Server_SetMovementProfile(EMovementProfile NewMovementProfile);
	UFUNCTION(Server, Reliable)
	void Server_SetRotationMode(ERotationMode NewRotationMode);
	UFUNCTION(Server, Reliable)
	void Server_SetGlobalStance(EPawnGlobalStance NewGlobalStance);
	UFUNCTION(Server, Reliable)
	void Server_SetLocomotionMode(ELocomotionMode NewLocomotionMode);
	UFUNCTION(Server, Reliable)
	void Server_SetOverlayState(EOverlayState NewOverlayState);

	/*******************
	* Getters
	******************/
	FORCEINLINE EMovementProfile GetMovementProfile() const { return MovementProfile; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ERotationMode GetRotationMode() const { return RotationMode; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EMovementAction GetMovementAction() const { return MovementAction; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ELocomotionMode GetLocomotionMode() const { return LocomotionMode; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EOverlayState GetOverlayState() const { return OverlayState; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EPawnGlobalStance GetMGlobalStance() const { return GlobalStance; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetSpeed() const { return Speed; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAimYaw() const { return AimYawRate; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsMoving() const { return bIsMoving; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsAlive() const { return HealthAttribute.GetDefaultObject()->GetHealth() > 0.f; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FRotator GetAimingRotation() const { return AimingRotation; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FVector GetPreviousVelocity() const { return PreviousVelocity; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FVector GetAcceleration() const { return Acceleration; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ABaseWeapon* GetCurrentWeapon() const { return CurrentWeapon; }
	FORCEINLINE UBaseMovementComponent* GetCustomMovementComponent() const
	{
		return StaticCast<UBaseMovementComponent*>(GetCharacterMovement());
	}
	FORCEINLINE virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return static_cast<UAbilitySystemComponent*>(AbilitySystemComponent);
	}
	
	UFUNCTION(BlueprintCallable)
	virtual bool IsAttack();

	UFUNCTION(BlueprintCallable)
	virtual float GetHealth();

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth();

	UFUNCTION(BlueprintCallable)
	virtual float GetDamage();


	void SetIsAttack(bool const bAttack) { bIsAttacking = bAttack; }
	/*************
	* Delegates
	************/
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnJumpedDelegate JumpedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnAttackDelegate NormalAttackDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnAttackDelegate ChargeAttackDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnMontageChangedDelegate MontageChanged;
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnWeaponChangedDelegate WeaponChangedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnCharacterDeath CharacterDeathDelegate;
	FOnCharacterDeathNative CharacterDeathNative;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnDeath();
	void OnDeathNative();
	
	void SetMovementModel();
	void WarpToTarget(FName WarpTargetName);
	
	FMovementProfileSettings GetTargetMovementSettings() const;

private:

	void SetEssentialVariables();
	void UpdateEssentialVariables(float DeltaTime);
		
};
