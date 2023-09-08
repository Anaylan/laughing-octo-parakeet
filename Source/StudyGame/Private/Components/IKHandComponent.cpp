// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/IKHandComponent.h"

#include "Character/BaseCharacter.h"

// Sets default values for this component's properties
UIKHandComponent::UIKHandComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UIKHandComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedCharacter = StaticCast<ABaseCharacter*>(GetOwner());
	if (!CachedCharacter.IsValid()) return;

	MeshComponent = StaticCast<USkeletalMeshComponent*>(CachedCharacter->GetMesh());
	if (!MeshComponent.IsValid()) return;
}

// Called every frame
void UIKHandComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CachedCharacter.IsValid()) return;
	if (!MeshComponent.IsValid()) return;
	
	if (!bCanUpdate) return;
	
	UpdateHandLocationAndRotation(HandSettings.LeftHandIK, HandSettings.WeaponSocketL, 
		HandValues.HandLocationL, HandValues.HandRotationL);
	UpdateHandLocationAndRotation(HandSettings.RightHandIK, HandSettings.WeaponSocketR, 
		HandValues.HandLocationR, HandValues.HandRotationR);
}

void UIKHandComponent::UpdateHandLocationAndRotation(FName HandSocket, FName WeaponSocket, FVector& CurHandLocation, FRotator& CurHandRotation)
{
	FVector HandLocation = MeshComponent->GetBoneLocation(HandSocket);
	FQuat HandRotation = MeshComponent->GetBoneQuaternion(HandSocket);
	
	FVector WeaponLocation = MeshComponent->GetSocketLocation(WeaponSocket);

	// GetDirector from hand to weapon
	FVector ArmDirection = WeaponLocation - HandLocation;
	ArmDirection.Normalize();

	CurHandRotation = FQuat::Slerp(HandRotation, FQuat::FindBetweenVectors(FVector::ForwardVector, ArmDirection),
		20.f).Rotator();
	CurHandLocation = HandLocation + (WeaponLocation - HandLocation);
}

