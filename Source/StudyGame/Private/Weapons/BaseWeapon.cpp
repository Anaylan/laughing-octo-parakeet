// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BaseWeapon.h"

#include "Character/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(Root, NAME_None);

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	CollisionComponent->SetupAttachment(MeshComponent);
	CollisionComponent->CanCharacterStepUpOn = ECB_Yes;
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{	
	Super::BeginPlay();
	bReplicates = true;
	
	GetMesh()->AddTickPrerequisiteActor(this);
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseWeapon::Equip(ACharacter* NewOwner)
{
	MeshComponent->SetVisibility(true);
	bEquipped = true;
	SetOwner(NewOwner);
}

void ABaseWeapon::UnEquip()
{
	MeshComponent->SetVisibility(false);
	bEquipped = false;
	SetOwner(nullptr);
}

bool ABaseWeapon::CanUsed()
{
	return !bEquipped;
}

float ABaseWeapon::GetDamage()
{
	return WeaponProperties.BaseDMG * WeaponProperties.DMGMultiplier;
}

