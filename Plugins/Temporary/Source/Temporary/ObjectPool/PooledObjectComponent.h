// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PooledObjectComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEMPORARY_API UPooledObjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    void Init(class UObjectPoolWorldSubsystem* Owner);

    // 자멸버튼
    UFUNCTION(BlueprintCallable)
    void RecycleSelf();

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    bool bIsPoolActive;

private:
    TObjectPtr<class UObjectPoolWorldSubsystem> ObjectPool;

    // 자멸버튼
    virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

		
};
