// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/IKFootComponent.h"

#include "Animation/CharacterAnimInstance.h"
#include "Character/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UIKFootComponent::UIKFootComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UIKFootComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());
	if (!CharacterOwner.IsValid()) return;
	
	MeshComponent = CharacterOwner->GetMesh();
	if (!MeshComponent.IsValid()) return;
}

// Called every frame
void UIKFootComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CharacterOwner.IsValid()) return;
	if (!MeshComponent.IsValid()) return;
	
	if (CharacterOwner->GetCharacterMovement()->IsMovingOnGround() && bActive)
	{
		UpdateFootTrace(LeftHitResult, LeftFootSocket);
		UpdateFootTrace(RightHitResult, RightFootSocket);

		UpdateFootOffset(LeftHitResult, LeftFootSocket, FootVariables.LeftFootOffset, DeltaTime, FootVariables.fFootHeight);
		UpdateFootOffset(RightHitResult, RightFootSocket, FootVariables.RightFootOffset, DeltaTime, FootVariables.fFootHeight);

		UpdateFootRotation(FootVariables.LeftFootRotation, LeftHitResult, DeltaTime);
		UpdateFootRotation(FootVariables.RightFootRotation, RightHitResult, DeltaTime);
		
		UpdatePelvisOffset(FootVariables.PelvisOffset, DeltaTime, FootVariables.LeftFootOffset, FootVariables.RightFootOffset, 20.f);
	}
}

void UIKFootComponent::UpdateFootTrace(FHitResult& HitResult, FName SocketName)
{
	const FVector SocketLocation = MeshComponent->GetSocketLocation(SocketName);

	const FVector Start = FVector(SocketLocation.X, SocketLocation.Y, CharacterOwner->GetActorLocation().Z);
	const FVector End = FVector(SocketLocation.X, SocketLocation.Y, CharacterOwner->GetActorLocation().Z - 
		(CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + FootVariables.fFootHeight + fTraceDistance));

	EDrawDebugTrace::Type DrawDebugTrace = bDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;

	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false, ActorsToIgnore, DrawDebugTrace, HitResult, true);
}

void UIKFootComponent::UpdatePelvisOffset(FVector& PelvisOffset, float DeltaTime, FVector LeftFootOffset,
	FVector RightFootOffset, float InterpSpeed)
{
	FVector TargetOffset = FVector::ZeroVector;
	if (CharacterOwner->GetCharacterMovement()->IsMovingOnGround())
	{
		TargetOffset = RightFootOffset.Z > LeftFootOffset.Z ? LeftFootOffset : RightFootOffset;
	}

	PelvisOffset = FMath::VInterpTo(PelvisOffset, TargetOffset, DeltaTime, InterpSpeed);
}

void UIKFootComponent::UpdateFootOffset(FHitResult HitResult, FName SocketName, FVector& CurFootOffset,
	float DeltaTime, float FootHeight)
{
	FVector TargetLocation;

	FVector CurLocation = MeshComponent->GetSocketLocation(SocketName);
	CurLocation.Z = MeshComponent->GetSocketLocation(UCharacterAnimInstance::NAME_RootBone).Z;

	if (CharacterOwner->GetCharacterMovement()->IsWalkable(HitResult))
	{
		TargetLocation = FVector(HitResult.ImpactNormal * FootHeight + HitResult.ImpactPoint) - (CurLocation + FVector(0.f, 0.f, FootHeight));
	}

	float InterpSpeed = CharacterOwner->GetCharacterMovement()->IsMovingOnGround() ? 15.f : 25.f;

	CurFootOffset = FMath::VInterpTo(CurFootOffset, TargetLocation, DeltaTime, InterpSpeed);
}

void UIKFootComponent::UpdateFootRotation(FRotator& CurFootRotation, FHitResult HitResult, float DeltaTime,
	float InterpSpeed)
{
	CurFootRotation = FMath::RInterpTo(CurFootRotation, NormalToRotation(HitResult.ImpactNormal), DeltaTime, InterpSpeed);
}

FRotator UIKFootComponent::NormalToRotation(FVector ImpactNormal)
{
	FRotator TargetRotation = FRotator::ZeroRotator;

	TargetRotation.Roll = FMath::RadiansToDegrees(FMath::Atan2(ImpactNormal.Y, ImpactNormal.Z));
	TargetRotation.Pitch = -FMath::RadiansToDegrees(FMath::Atan2(ImpactNormal.X, ImpactNormal.Z));

	return TargetRotation;
}
 
