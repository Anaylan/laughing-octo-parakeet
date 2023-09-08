// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Library/CharacterStructLibrary.h"
#include "IKHandComponent.generated.h"


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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bCanUpdate:1;

	TWeakObjectPtr<ABaseCharacter> CachedCharacter = nullptr;
	TWeakObjectPtr<USkeletalMeshComponent> MeshComponent = nullptr; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FIKHandSettings HandSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FIKHandValues HandValues;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateHandLocationAndRotation(FName HandSocket, FName WeaponSocket, FVector& CurHandLocation, FRotator& CurHandRotation);

	FIKHandValues GetProperties() const { return HandValues; }
};
