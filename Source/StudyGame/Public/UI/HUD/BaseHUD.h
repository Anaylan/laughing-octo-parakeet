// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseHUD.generated.h"

class UBaseWidget;
class ABaseCharacter;

/**
 * 
 */
UCLASS(Abstract)
class STUDYGAME_API ABaseHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<ABaseCharacter> CharacterOwner;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (AllowPrivateAccess = true))
	TArray<UBaseWidget*> CreatedWidgets;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TArray<TSubclassOf<UBaseWidget>> AllWidgets;
	
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	
};
