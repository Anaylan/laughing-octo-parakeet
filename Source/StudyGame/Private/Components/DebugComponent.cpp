// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DebugComponent.h"

#include "Character/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UDebugComponent::UDebugComponent()
{
}

// Called when the game starts
void UDebugComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CachedCharacter = Cast<ABaseCharacter>(GetOwner());
}

void UDebugComponent::DrawArrow(FVector Start, FVector End, float ArrowSize, FLinearColor LinearColor, float Thickness) const
{
#if WITH_EDITOR
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), Start, End, ArrowSize, LinearColor, 0.f, Thickness);
#endif
}

void UDebugComponent::DrawVelocityDirection(float ArrowSize, FLinearColor LinearColor, float Thickness, float OffsetZ)
{
#if WITH_EDITOR
	if (!CachedCharacter.IsValid()) return;

	const FVector Start = CachedCharacter->GetActorLocation() - FVector(0.f, 0.f,
		CachedCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OffsetZ);

	const float ClampedVelocity = UKismetMathLibrary::MapRangeClamped(CachedCharacter->GetVelocity().Size2D(),
		0.f,50.f, 50.f, 100.f);
	
	FVector End = Start + FRotationMatrix(CachedCharacter->GetActorRotation()).GetUnitAxis(EAxis::X).GetSafeNormal() * ClampedVelocity;
	
	DrawArrow(Start, End, ArrowSize, LinearColor, Thickness);
#endif
}

void UDebugComponent::DrawControlRotation(float ArrowSize, FLinearColor LinearColor, float Thickness, float OffsetZ)
{
#if WITH_EDITOR
	if (!CachedCharacter.IsValid()) return;

	const FVector Start = CachedCharacter->GetActorLocation() - FVector(0.f, 0.f,
		CachedCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OffsetZ);
	
	FVector End = Start + FRotationMatrix(CachedCharacter->GetControlRotation()).GetUnitAxis(EAxis::X).GetSafeNormal2D() * 50.f;
	
	DrawArrow(Start, End, ArrowSize, LinearColor, Thickness);
#endif
}

void UDebugComponent::DrawTargetVelocityDirection(float ArrowSize, FLinearColor LinearColor, float Thickness,
	float OffsetZ)
{
#if WITH_EDITOR
	if (!CachedCharacter.IsValid()) return;

	const FVector Start = CachedCharacter->GetActorLocation() - FVector(0.f, 0.f,
		CachedCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OffsetZ);

	const float ClampedVelocity = UKismetMathLibrary::MapRangeClamped(CachedCharacter->GetVelocity().Size2D(),
		0.f,50.f, 50.f, 100.f);
	
	FVector End = Start + FRotationMatrix(CachedCharacter->GetVelocity().Rotation()).GetUnitAxis(EAxis::X).GetSafeNormal() * ClampedVelocity;
	
	DrawArrow(Start, End, ArrowSize, LinearColor, Thickness);
#endif
}
