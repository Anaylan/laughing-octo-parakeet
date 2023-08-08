
#include "Character/Abilities/PunchGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/BaseCharacter.h"
#include "Character/Abilities/Tasks/AbilityTask_SuccessFailEvent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapons/BaseWeapon.h"

UPunchGameplayAbility::UPunchGameplayAbility()
{
	AbilityTypeID = EAbilityTypeInputID::IA_NormalAttack;
}

void UPunchGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* SourceActor = GetAvatarActorFromActorInfo();
	if (!SourceActor)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ABaseCharacter* SourceCharacter = Cast<ABaseCharacter>(SourceActor);
	if (!SourceCharacter)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ABaseWeapon* SourceWeapon = SourceCharacter->GetCurrentWeapon();

	SourceCharacter->PlayAttackAnimation(MontageSettings);
	// Search overlapped weapon mesh actors
	// TODO: Not overlapped actors, maybe collision trouble
	TArray<AActor*> OverlappedActors{};
	SourceWeapon->GetCollision()->GetOverlappingActors(OverlappedActors, ABaseCharacter::StaticClass());
	// OverlappedActors.Remove(SourceActor);

	CheckOverlap(SourceActor, SourceWeapon);
	// Complete ability after operations
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UPunchGameplayAbility::CheckOverlap_Implementation(AActor* SourceActor, const ABaseWeapon* SourceWeapon)
{
	TArray<AActor*> OverlappedActors{};
	SourceWeapon->GetCollision()->GetOverlappingActors(OverlappedActors);
	OverlappedActors.Remove(SourceActor);
	UE_LOG(LogTemp, Warning, TEXT("Function running"))
	if (OverlappedActors.Num() > 0)
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Display, TEXT("Found actors"))
#endif
		for (AActor* Actor : OverlappedActors)
		{

			if (Actor &&
				Actor != SourceActor &&
				Actor->ActorHasTag("Enemy") &&
				UKismetSystemLibrary::DoesImplementInterface(
					Actor, UAbilitySystemInterface::StaticClass())
					)
			{
#if WITH_EDITOR
				UE_LOG(LogTemp, Display, TEXT("Found actor"))
#endif
				// don't punch if dead
				if (Cast<IAbilitySystemInterface>(Actor)->GetAbilitySystemComponent()->HasMatchingGameplayTag(
					FGameplayTag::RequestGameplayTag("Gameplay.Status.IsDead")))
				{
#if WITH_EDITOR
					UE_LOG(LogTemp, Display, TEXT("Found IsDead"))
#endif
					continue;
				}

#if WITH_EDITOR
				UE_LOG(LogTemp, Display, TEXT("Send Data to server"))
#endif
				// const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Weapon.Hit"));
				// FGameplayEventData Payload = FGameplayEventData();
				// Payload.Instigator = SourceActor;
				// Payload.Target = Actor;
				// Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Actor);
				// UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceActor, Tag, Payload);
			}
		}
	}
}