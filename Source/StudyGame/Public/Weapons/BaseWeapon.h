// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Library/WeaponStructLibrary.h"
#include "BaseWeapon.generated.h"

struct FWeaponProperties;
class ABaseCharacter;
class UCapsuleComponent;

UCLASS(Abstract)
class STUDYGAME_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bEquipped = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> MeshComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UCapsuleComponent> CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	FWeaponProperties WeaponProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	FName SocketInCharacter = FName(TEXT("Weapon_R"));
	
public:

	void Equip(ACharacter* NewOwner);
	void UnEquip();
	
	virtual float GetDamage();

	FORCEINLINE EWeaponRange GetRangeType() const { return WeaponProperties.Range; }
	FORCEINLINE EWeaponType GetType() const { return WeaponProperties.Type; }
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return MeshComponent; }
	FORCEINLINE UCapsuleComponent* GetCollision() const { return CollisionComponent; }
	FORCEINLINE FIKWeaponProperties GetIKProperties() const { return IKProperties; }
	FORCEINLINE FName GetWeaponName() const { return WeaponProperties.Name; }
	FORCEINLINE FName GetTargetSocketName() const { return SocketInCharacter; }
private:
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FIKWeaponProperties IKProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration", meta = (AllowPrivateAccess = true))
	FTransform PlacementTransform;
};
