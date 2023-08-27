// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TP_Character.h"

#include "Components/CapsuleComponent.h"
#include "Components/DebugComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATP_Character::ATP_Character(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.f);

	
	DebugComponent = CreateDefaultSubobject<UDebugComponent>(TEXT("DebugComponent"));
}

// Called when the game starts or when spawned
void ATP_Character::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATP_Character::EnableRagdoll()
{
	Super::EnableRagdoll();

	if (!IsAlive())
	{
		PrimaryActorTick.bCanEverTick = false;
		UCapsuleComponent* CapsuleComp = StaticCast<UCapsuleComponent*>(GetCapsuleComponent());
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

		SetReplicateMovement(false);
		DetachFromControllerPendingDestroy();

		// SetLifeSpan(5.f);

	}
}

// Called every frame
void ATP_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void ATP_Character::SprintAction()
{
	Super::SprintAction();
	
	// if (MovementProfile == EMovementProfile::Sprint)
	// {
	// 	InterruptMontage();
	// }
}

void ATP_Character::InterruptMontage()
{
	if (!IsPlayingAnimation() || GetMovementProfile() != EMovementProfile::Sprint) return;

	StopAnimMontage();
}
