#pragma once

#include "CoreMinimal.h"
#include "Library/CharacterEnumLibrary.h"
#include "Library/CharacterStructLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "BaseMovementComponent.generated.h"

class ABaseCharacter;

UCLASS()
class STUDYGAME_API UBaseMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	class FSavedMove_Base final : public FSavedMove_Character
	{
		typedef FSavedMove_Character Super;

		// Flags
		uint8 bSavedMovementProfileUpdate:1;
		uint8 bSavedRotationModeUpdate:1;
		
		uint8 bSavedWantsToSlide:1;
		TEnumAsByte<enum EMovementProfile> SavedMovementProfile{ EMovementProfile::Running };
		TEnumAsByte<enum ERotationMode> SavedRotationMode{ ERotationMode::Looking };
		
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	class FNetworkPredictionData_Client_Base : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Base(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FMovementProfileSettings MovementSettings;

	UPROPERTY(Transient)
	TObjectPtr<ABaseCharacter> BaseCharacterOwner = nullptr;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<enum EMovementProfile> SafeMovementProfile;
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<enum ERotationMode> SafeRotationMode;
	
	bool bSafeMovementSettingsUpdate = false;
	bool bSafeRotationModeUpdate = false;
	bool bSafeWantsToSlide = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration", meta = (AllowPrivateAccess = true))
	FSlideSettings SlideSettings;
	
protected:
	
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void InitializeComponent() override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void PhysWalking(float deltaTime, int32 Iterations) override;

	float GetMappedSpeed() const;
	
private:

	void PhysSlide(float DeltaTime, int32 Iterations);
	void ExitSlide();

	void EnterSlide(EMovementMode PrevMovementMode, ECustomMovementMode PrevCustomMode);
	
public:
	
	UBaseMovementComponent(const FObjectInitializer& ObjectInitializer);

	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual float GetMaxAcceleration() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	void SetMovementProfile(EMovementProfile NewMovementProfile);

	virtual bool IsMovingOnGround() const override;
	virtual bool CanCrouchInCurrentState() const override;

	UFUNCTION(Server, Reliable)
	void Server_SetMovementProfile(EMovementProfile NewMovementProfile);
	UFUNCTION(Server, Reliable)
	void Server_SetRotationMode(ERotationMode NewRotationMode);
	
	void SetMovementSettings(FMovementProfileSettings NewMovementSettings)
	{
		MovementSettings = NewMovementSettings;
		bSafeMovementSettingsUpdate = true;
	}

	bool CanSlide() const;

	UFUNCTION(BlueprintPure)
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
	
	void SlideTriggered()
	{
		bSafeWantsToSlide = !bSafeWantsToSlide;
	}

	void SetRotationMode(ERotationMode NewRotationMode);
};

