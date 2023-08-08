// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/StepAnimNotify.h"

#include "Kismet/KismetSystemLibrary.h"

UStepAnimNotify::UStepAnimNotify()
{
#if WITH_EDITOR
	bShouldFireInEditor = false;
#endif
}

void UStepAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	const AActor* Owner = MeshComp->GetOwner();
	if (!Owner)	return;

	const UWorld* World = MeshComp->GetWorld();
	check(World);
	
	FVector FootLocation = MeshComp->GetSocketLocation(FootBoneName);
	FVector EndTrace = FootLocation - Owner->GetActorUpVector() * 30.f;
	
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(MeshComp->GetOwner());
	
	if (UKismetSystemLibrary::LineTraceSingle(World, FootLocation, EndTrace,
		UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
		ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
	{
		if (!HitResult.PhysMaterial.Get()) return;
		UKismetSystemLibrary::DrawDebugSphere(World, HitResult.ImpactPoint, 10, 6, FLinearColor::Red, 10);
	}
}

FTransform UStepAnimNotify::SetHitboxLocation(FString boneName, USkeletalMeshComponent* MeshComp){
	FName convertedBoneName = FName(*boneName);
	FTransform desiredLocation = MeshComp->GetBodyInstance(convertedBoneName)->GetUnrealWorldTransform();
	return desiredLocation;
}