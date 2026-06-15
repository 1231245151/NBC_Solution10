// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PooledObjectData.generated.h"


USTRUCT(BlueprintType)
struct FPooledObjectData

{
    GENERATED_BODY()

    FPooledObjectData()
    {
        ActorTemplate = nullptr;
        Poolsize = 1;
        bCanGrow = false;
        ActorName = "default";
    }

public:
    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> ActorTemplate;

    // 오브젝트 개수
    UPROPERTY(EditAnywhere)
    int32 Poolsize;

    // 만일 풀링 개수이상을 요청할경우 더만들거냐 안할거냐
    UPROPERTY(EditAnywhere)
    bool bCanGrow;

    // 갖고있는 액터 이름
    UPROPERTY(EditAnywhere)
    FString ActorName;



};
