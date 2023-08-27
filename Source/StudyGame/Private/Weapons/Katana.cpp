// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Katana.h"

// Sets default values
AKatana::AKatana()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponProperties.Range = EWeaponRange::Melee;
	WeaponProperties.Type = EWeaponType::Katana;
}

// Called when the game starts or when spawned
void AKatana::BeginPlay()
{
	Super::BeginPlay();
}