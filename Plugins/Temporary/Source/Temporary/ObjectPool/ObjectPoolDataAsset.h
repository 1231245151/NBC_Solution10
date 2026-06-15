// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PooledObjectData.h"
#include "ObjectPoolDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TEMPORARY_API UObjectPoolDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FPooledObjectData> PoolDataArray;
};
