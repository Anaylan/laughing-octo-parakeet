// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasePlayerController.h"
#include "Character/BaseCharacter.h"

#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputSubsystems.h"
#include "Character/BasePlayerCameraManager.h"
#include "Library/AbilityEnumLibrary.h"
#include "Components/BaseAbilitySystemComponent.h"


ABasePlayerController::ABasePlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	PlayerCameraManagerClass = ABasePlayerCameraManager::StaticClass();

}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->ClearActionBindings();
		EnhancedInputComponent->ClearDebugKeyBindings();
		EnhancedInputComponent->ClearAxisBindings();
		
		BindInputs(DefaultInputMapping);
		
		if (PossessedCharacter && PossessedCharacter->GetAbilitySystemComponent())
		{
			const FGameplayAbilityInputBinds Binds(
				"Confirm",
				"Canceled",
				FTopLevelAssetPath(GetPathNameSafe(
					UClass::TryFindTypeSlow<UEnum>("/Script/StudyGame.EAbilityTypeInputID"))),
				static_cast<int32>(EAbilityTypeInputID::Confirm),
				static_cast<int32>(EAbilityTypeInputID::Canceled));

			UE_LOG(LogTemp, Warning, TEXT("BindNames: %s"), *Binds.EnumPathName.ToString())
			PossessedCharacter->GetAbilitySystemComponent()->BindAbilityActivationToInputComponent(EnhancedInputComponent, Binds);
		}
	}
}

void ABasePlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	PossessedCharacter = StaticCast<ABaseCharacter*>(GetPawn());

	SetupInput();
}

void ABasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessedCharacter = Cast<ABaseCharacter>(InPawn);

	if (PossessedCharacter)
	{
		PossessedCharacter->GetAbilitySystemComponent()->InitAbilityActorInfo(PossessedCharacter, InPawn);
	}
	SetupInput();
	SetupCamera();
}

void ABasePlayerController::BindInputs(const UInputMappingContext* MappingContext)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (!MappingContext) return;
		TArray<FEnhancedActionKeyMapping> KeyMappings = MappingContext->GetMappings();

		if (KeyMappings.Num() > 0)
		{
			for (FEnhancedActionKeyMapping KeyMapping: KeyMappings)
			{
				EnhancedInputComponent->BindAction(KeyMapping.Action, ETriggerEvent::Triggered,
					Cast<UObject>(this), KeyMapping.Action->GetFName());
			}
		}
	}
}

void ABasePlayerController::SetupInput() const
{
	if (PossessedCharacter)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			FModifyContextOptions ContextOptions;
			ContextOptions.bForceImmediately = true;
			
			Subsystem->AddMappingContext(DefaultInputMapping, 1, ContextOptions);
		}
	}
}

void ABasePlayerController::SetupCamera() const
{
}

void ABasePlayerController::IA_Move(const FInputActionValue& Value) const
{
	if (PossessedCharacter)
	{
		PossessedCharacter->MoveAction(Value.Get<FVector2D>());
	}
}

void ABasePlayerController::IA_Look(const FInputActionValue& Value) const
{
	if (PossessedCharacter)
	{
		PossessedCharacter->LookAction(Value.Get<FVector2D>());
	}
}

void ABasePlayerController::IA_Jump(const FInputActionValue& Value) const
{
	if (PossessedCharacter)
	{
		PossessedCharacter->JumpAction(Value.Get<bool>());
	}
}

void ABasePlayerController::IA_Sprint(const FInputActionValue& Value) const
{
	if (PossessedCharacter)
	{
		PossessedCharacter->SprintAction();
	}
}

void ABasePlayerController::IA_Walk(const FInputActionValue& Value) const
{
	if (PossessedCharacter)
	{
		PossessedCharacter->WalkAction();
	}
}

void ABasePlayerController::IA_Aim(const FInputActionValue& Value) const
{
	if (PossessedCharacter)
	{
		PossessedCharacter->AimAction();
	}
}

void ABasePlayerController::IA_NormalAttack(const FInputActionValue& Value) const
{
	if (PossessedCharacter)
	{
		PossessedCharacter->UseAbility(Value.Get<bool>(), EAbilityTypeInputID::IA_NormalAttack);
	}
}

void ABasePlayerController::IA_NextWeapon(const FInputActionValue& Value) const
{
	if (PossessedCharacter)
	{
		PossessedCharacter->NextWeapon();
	}
}

void ABasePlayerController::IA_PrevWeapon(const FInputActionValue& Value) const
{
	if (PossessedCharacter)
	{
		PossessedCharacter->PrevWeapon();
	}
}
