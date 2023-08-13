// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/AttackNotifyState.h"

#include "Character/BaseCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapons/BaseWeapon.h"

void UAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner)) return;
	
	ABaseCharacter* Character = StaticCast<ABaseCharacter*>(Owner);
	if (!IsValid(Character)) return;

	if (Character->GetCurrentWeapon())
	{
#if UE_EDITOR
		UE_LOG(LogTemp, Log, TEXT("Notify begin"));
#endif
		Character->GetCurrentWeapon()->AllowOverlapDelegate.Broadcast(true);
	}

	Character->SetIsAttack(true);

}

void UAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner)) return;
	
	ABaseCharacter* Character = StaticCast<ABaseCharacter*>(Owner);
	if (!IsValid(Character)) return;

	if (Character->GetCurrentWeapon())
	{
#if UE_EDITOR
		UE_LOG(LogTemp, Log, TEXT("Notify end"));
#endif
		Character->GetCurrentWeapon()->AllowOverlapDelegate.Broadcast(false);
	}
	Character->SetIsAttack(false);
}
