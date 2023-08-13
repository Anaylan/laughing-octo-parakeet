// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Katana.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"
#include "Character/BaseCharacter.h"
#include "Components/BaseAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AKatana::AKatana()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponProperties.Range = EWeaponRange::Melee;
	WeaponProperties.Type = EWeaponType::Katana;
}

void AKatana::SetOverlap(bool bNewOverlapValue)
{
	bCanOverlap = bNewOverlapValue;
}

// Called when the game starts or when spawned
void AKatana::BeginPlay()
{
	Super::BeginPlay();

	AllowOverlapDelegate.AddUObject(this, &ThisClass::SetOverlap);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
}

void AKatana::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bCanOverlap) return;
// 	UE_LOG(LogTemp, Warning, TEXT("Overlap method called"))
// 	UKismetSystemLibrary::DrawDebugSphere(GetMesh()->GetWorld(), SweepResult.ImpactPoint, 20, 6, FLinearColor::Blue, 10);
// 	if (OtherActor == nullptr || !OtherActor->ActorHasTag("Enemy") || OtherActor == GetOwner())
// 	{
// #if WITH_EDITOR
// 		UE_LOG(LogTemp, Warning, TEXT("Send empty payload"))
// #endif
// 		FGameplayEventData Payload = FGameplayEventData();
// 		Payload.Instigator = GetOwner();
// 		Payload.TargetData = FGameplayAbilityTargetDataHandle();
// 		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), FGameplayTag::RequestGameplayTag(
// 			FName("Weapon.NoHit")), Payload);
// 		return;
// 	}
//
// 	// Check if the OtherActor implements the AbilitySystemInterface
// 	if (OtherActor->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
// 	{
// 		if (IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OtherActor))
// 		{
// 			UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
// 			if (AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Gameplay.Status.IsDead"))))
// 			{
// 				FGameplayEventData Payload = FGameplayEventData();
// 				Payload.Instigator = GetOwner();
// 				Payload.TargetData = FGameplayAbilityTargetDataHandle();
// 				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), FGameplayTag::RequestGameplayTag(
// 					FName("Weapon.NoHit")), Payload);
// 				return;
// 			}
// 		}
// 	}
// 	
// #if WITH_EDITOR
// 	UE_LOG(LogTemp, Log, TEXT("Send payload"))
// #endif
// 	
// 	FGameplayEventData Payload = FGameplayEventData();
// 	Payload.Instigator = GetOwner();
// 	Payload.Target = OtherActor;
// 	Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(OtherActor);
// 	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), FGameplayTag::RequestGameplayTag(
// 		FName("Weapon.Hit")), Payload);
}
