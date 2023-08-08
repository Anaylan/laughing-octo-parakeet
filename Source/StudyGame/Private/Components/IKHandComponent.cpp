// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/IKHandComponent.h"

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
}


// Called every frame
void UIKHandComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UIKHandComponent::UpdateHandTransform(FName SocketName, float DeltaTime)
{
	// FTransform WeaponTransform = MeshComponent->GetSocketTransform(SocketName);
}

