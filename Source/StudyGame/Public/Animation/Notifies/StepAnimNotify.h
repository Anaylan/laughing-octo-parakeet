// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "StepAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class STUDYGAME_API UStepAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	FName FootBoneName = NAME_None;

public:
	UStepAnimNotify();
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	FTransform SetHitboxLocation(FString boneName, USkeletalMeshComponent* MeshComp);
};
