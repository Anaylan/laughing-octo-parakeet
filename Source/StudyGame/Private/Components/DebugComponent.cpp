// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DebugComponent.h"

#include "Character/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UDebugComponent::UDebugComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UDebugComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CachedCharacter = Cast<ABaseCharacter>(GetOwner());
}


// Called every frame
void UDebugComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDebugComponent::DrawArrow(FVector Start, FVector End, float ArrowSize, FLinearColor LinearColor, float Thickness) const
{
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), Start, End, ArrowSize, LinearColor, 0.f, Thickness);
}

void UDebugComponent::DrawVelocityDirection(float ArrowSize, FLinearColor LinearColor, float Thickness, float OffsetZ)
{
	if (!CachedCharacter.IsValid()) return;

	const FVector Start = CachedCharacter->GetActorLocation() - FVector(0.f, 0.f,
		CachedCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OffsetZ);

	const float ClampedVelocity = UKismetMathLibrary::MapRangeClamped(CachedCharacter->GetVelocity().Size2D(),
		0.f,50.f, 50.f, 100.f);
	
	FVector End = Start + FRotationMatrix(CachedCharacter->GetActorRotation()).GetUnitAxis(EAxis::X).GetSafeNormal() * ClampedVelocity;
	
	DrawArrow(Start, End, ArrowSize, LinearColor, Thickness);
}

void UDebugComponent::DrawControlRotation(float ArrowSize, FLinearColor LinearColor, float Thickness, float OffsetZ)
{
	if (!CachedCharacter.IsValid()) return;

	const FVector Start = CachedCharacter->GetActorLocation() - FVector(0.f, 0.f,
		CachedCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OffsetZ);
	
	FVector End = Start + FRotationMatrix(CachedCharacter->GetControlRotation()).GetUnitAxis(EAxis::X).GetSafeNormal2D() * 50.f;
	
	DrawArrow(Start, End, ArrowSize, LinearColor, Thickness);
}

void UDebugComponent::DrawTargetVelocityDirection(float ArrowSize, FLinearColor LinearColor, float Thickness,
	float OffsetZ)
{
	if (!CachedCharacter.IsValid()) return;

	const FVector Start = CachedCharacter->GetActorLocation() - FVector(0.f, 0.f,
		CachedCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OffsetZ);

	const float ClampedVelocity = UKismetMathLibrary::MapRangeClamped(CachedCharacter->GetVelocity().Size2D(),
		0.f,50.f, 50.f, 100.f);
	
	FVector End = Start + FRotationMatrix(CachedCharacter->GetVelocity().Rotation()).GetUnitAxis(EAxis::X).GetSafeNormal() * ClampedVelocity;
	
	DrawArrow(Start, End, ArrowSize, LinearColor, Thickness);
}
