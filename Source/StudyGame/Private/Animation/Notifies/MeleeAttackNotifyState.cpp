// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/MeleeAttackNotifyState.h"

#include "Character/BaseCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UMeleeAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	ABaseCharacter* CachedCharacter = Cast<ABaseCharacter>(Owner);
	if (!CachedCharacter) return;
	if (CachedCharacter->HasAuthority())
	{
		CachedCharacter->Server_SetIsPlayingAnimation(true);
	}
}

void UMeleeAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;
	
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	
	ABaseCharacter* CachedCharacter = Cast<ABaseCharacter>(Owner);
	if (!CachedCharacter) return;
	
	if (CachedCharacter->HasAuthority())
	{
		CachedCharacter->Server_SetIsPlayingAnimation(false);
	}
	CachedCharacter = nullptr;
}

void UMeleeAttackNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
		
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	
	ABaseCharacter* CachedCharacter = Cast<ABaseCharacter>(Owner);
	if (!CachedCharacter) return;
	
	if (CachedCharacter && CachedCharacter->IsLocallyControlled())
	{
		CachedCharacter->HandlePunch();
	}
}