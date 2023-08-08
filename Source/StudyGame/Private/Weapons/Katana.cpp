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
	
	// BladeCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("StartBlade"));
	// BladeCapsule->SetupAttachment(MeshComponent);
	// BladeCapsule->SetCollisionProfileName(TEXT("OverlapAll"));

	// MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// MeshComponent->SetGenerateOverlapEvents(true);
	// MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap); // Allow overlap to all collision channels
}

void AKatana::SetOverlap(bool bNewOverlapValue)
{
	if (bCanOverlap != bNewOverlapValue)
	{
		bCanOverlap = bNewOverlapValue;
	}
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

	UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherActor->GetName());
	if (!OtherActor || OtherActor == GetOwner() ||
		OtherActor->IsA(ABaseWeapon::StaticClass()) || !OtherComp
		|| !OtherActor->ActorHasTag("Enemy"))
	{
		UE_LOG(LogTemp, Warning, TEXT("Send empty payload"))
		FGameplayEventData Payload = FGameplayEventData();
		Payload.Instigator = GetOwner();
		Payload.TargetData = FGameplayAbilityTargetDataHandle();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), FGameplayTag::RequestGameplayTag(
			FName("Weapon.NoHit")), Payload);
		return;
	}

	ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());

	if (!IsValid(CharacterOwner))
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("Send empty payload"))
#endif
		FGameplayEventData Payload = FGameplayEventData();
		Payload.Instigator = CharacterOwner;
		Payload.TargetData = FGameplayAbilityTargetDataHandle();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(CharacterOwner, FGameplayTag::RequestGameplayTag(
			FName("Weapon.NoHit")), Payload);
		return;
	}
	
	// don't punch if dead
	if (Cast<IAbilitySystemInterface>(OtherActor)->GetAbilitySystemComponent()->HasMatchingGameplayTag(
	FGameplayTag::RequestGameplayTag(
		FName("Gameplay.Status.IsDead"))))
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Log, TEXT("Found IsDead"))
#endif
	FGameplayEventData Payload = FGameplayEventData();
	Payload.Instigator = CharacterOwner;
	Payload.TargetData = FGameplayAbilityTargetDataHandle();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(CharacterOwner, FGameplayTag::RequestGameplayTag(
		FName("Weapon.NoHit")), Payload);
	return;
	}

#if WITH_EDITOR
		UE_LOG(LogTemp, Log, TEXT("Send payload"))
#endif
	FGameplayEventData Payload = FGameplayEventData();
	Payload.Instigator = CharacterOwner;
	Payload.Target = CharacterOwner;
	Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(OtherActor);
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(CharacterOwner, FGameplayTag::RequestGameplayTag(
		FName("Weapon.Hit")), Payload);
}
