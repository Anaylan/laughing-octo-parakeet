// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BaseWeapon.h"

#include "Character/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
	bReplicates = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(Root, NAME_None);

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(MeshComponent);
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{	
	Super::BeginPlay();
	bReplicates = true;
}

void ABaseWeapon::Equip(ACharacter* NewOwner)
{
	MeshComponent->SetVisibility(true);
	SetOwner(NewOwner);
}

void ABaseWeapon::UnEquip()
{
	MeshComponent->SetVisibility(false);
	SetOwner(nullptr);
}

float ABaseWeapon::GetDamage()
{
	return WeaponProperties.BaseDMG;
}

