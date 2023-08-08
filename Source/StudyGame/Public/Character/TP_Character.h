// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "TP_Character.generated.h"

class UDebugComponent;

UCLASS()
class STUDYGAME_API ATP_Character : public ABaseCharacter
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TObjectPtr<UDebugComponent> DebugComponent = nullptr;
	
public:
	// Sets default values for this character's properties
	ATP_Character(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EnableRagdoll() override;
	
	void InterruptMontage();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SprintAction() override;
};


