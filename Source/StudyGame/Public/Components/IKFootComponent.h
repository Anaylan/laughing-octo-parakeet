// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Library/CharacterStructLibrary.h"
#include "IKFootComponent.generated.h"

class ABaseCharacter;
class USkeletalMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STUDYGAME_API UIKFootComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bActive = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FName LeftFootSocket = FName(TEXT("ik_foot_l"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FName RightFootSocket = FName(TEXT("ik_foot_r"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float fTraceDistance = 34.f;

	TWeakObjectPtr<ABaseCharacter> CharacterOwner = nullptr;
	TWeakObjectPtr<USkeletalMeshComponent> MeshComponent = nullptr;

	FHitResult LeftHitResult;
	FHitResult RightHitResult;

	FIKFootProperties FootVariables;
	
public:	
	// Sets default values for this component's properties
	UIKFootComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void UpdateFootTrace(FHitResult& HitResult, FName SocketName);
	void UpdateFootOffset(FHitResult HitResult, FName SocketName, FVector& CurFootOffset, float DeltaTime, float FootHeight);
	void UpdateFootRotation(FRotator& CurFootRotation, FHitResult HitResult, float DeltaTime, float InterpSpeed = 10.f);
	
	void UpdatePelvisOffset(FVector& PelvisOffset, float DeltaTime, FVector LeftFootOffset, FVector RightFootOffset, float InterpSpeed = 20.f);

	void SetActiveIK(bool bNewActive) { bActive = bNewActive; }
	void SetDebugging(bool bNewDebug) { bDebug = bNewDebug; }
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FIKFootProperties GetIKProperties() const { return FootVariables; }
	
	FRotator NormalToRotation(FVector ImpactNormal);

};
