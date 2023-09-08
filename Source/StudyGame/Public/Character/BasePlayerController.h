// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class UInputMappingContext;
class ABaseCharacter;
struct FInputActionActionValue;

/**
 *
 */
UCLASS()
class STUDYGAME_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	// Binding functions
	virtual void SetupInputComponent() override;

	virtual void OnRep_Pawn() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABaseCharacter> PossessedCharacter = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input | Mapping Context")
	TObjectPtr<UInputMappingContext> DefaultInputMapping = nullptr;

public:
	ABasePlayerController();

protected:
	/***********
	 * Methods
	 **********/
	void BindInputs(const UInputMappingContext* MappingContext);

	void SetupInput() const;
	void SetupCamera() const;

	/***********
	 * Actions
	 **********/
	UFUNCTION(BlueprintCallable)
	void IA_Move(const FInputActionValue& Value) const;

	UFUNCTION(BlueprintCallable)
	void IA_Look(const FInputActionValue& Value) const;

	UFUNCTION(BlueprintCallable)
	void IA_Jump(const FInputActionValue& Value) const;

	UFUNCTION(BlueprintCallable)
	void IA_Sprint(const FInputActionValue& Value) const;

	UFUNCTION(BlueprintCallable)
	void IA_Walk(const FInputActionValue& Value) const;

	UFUNCTION(BlueprintCallable)
	void IA_Aim(const FInputActionValue& Value) const;

	UFUNCTION(BlueprintCallable)
	void IA_NormalAttack(const FInputActionValue& Value) const;

	UFUNCTION(BlueprintCallable)
	void IA_NextWeapon(const FInputActionValue& Value) const;

	UFUNCTION(BlueprintCallable)
	void IA_PrevWeapon(const FInputActionValue& Value) const;
	
	UFUNCTION(BlueprintCallable)
	void IA_ChargeAttack(const FInputActionValue& Value) const;
	
	UFUNCTION(BlueprintCallable)
	void IA_JumpAttack(const FInputActionValue& Value) const;

	UFUNCTION(BlueprintCallable)
	void IA_Dash(const FInputActionValue& Value) const;
	// Attributes
	

};