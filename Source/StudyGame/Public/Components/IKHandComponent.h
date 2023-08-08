// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IKHandComponent.generated.h"


class UCombatComponent;
class ABaseCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STUDYGAME_API UIKHandComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this component's properties
	UIKHandComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TWeakObjectPtr<ABaseCharacter> CachedCharacter = nullptr;
	TWeakObjectPtr<USkeletalMeshComponent> MeshComponent = nullptr; 
	TWeakObjectPtr<UCombatComponent> CombatComponent = nullptr;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateHandTransform(FName SocketName, float DeltaTime);
};
