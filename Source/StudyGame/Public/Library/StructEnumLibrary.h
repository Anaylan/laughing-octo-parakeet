// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StructEnumLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STUDYGAME_API UStructEnumLibrary : public UObject
{
	GENERATED_BODY()

public:

	template<typename T>
	static FORCEINLINE FString EnumToString(T InValue)
	{
		return StaticEnum<T>()->GetNameStringByValue(static_cast<int64>(InValue));
	}

	template<typename T>
	static FORCEINLINE FString EnumToIndex(T InValue)
	{
		return StaticEnum<T>()->GetNameStringByIndex(static_cast<int64>(InValue));
	}

};
