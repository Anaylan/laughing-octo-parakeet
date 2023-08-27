// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DebugComponent.generated.h"


class ABaseCharacter;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STUDYGAME_API UDebugComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDebugComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TWeakObjectPtr<ABaseCharacter> CachedCharacter = nullptr;
	
public:	

	void DrawArrow(FVector Start, FVector End, float ArrowSize, FLinearColor LinearColor, float Thickness) const;
	
	UFUNCTION(BlueprintCallable)
	void DrawVelocityDirection(float ArrowSize, FLinearColor LinearColor, float Thickness, float OffsetZ = 0.f);
		
    UFUNCTION(BlueprintCallable)
	void DrawControlRotation(float ArrowSize, FLinearColor LinearColor, float Thickness, float OffsetZ = 0.f);
        		
    UFUNCTION(BlueprintCallable)
	void DrawTargetVelocityDirection(float ArrowSize, FLinearColor LinearColor, float Thickness, float OffsetZ = 0.f);
};
