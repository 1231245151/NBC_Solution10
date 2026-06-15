// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "ObjectPoolDataAsset.h"

#include "ObjectPoolWorldSubsystem.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPoolerCleanup);

USTRUCT(BlueprintType)
struct FObjectPoolComponentList
{
    GENERATED_BODY()

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TArray<TObjectPtr<class UPooledObjectComponent>> PooledObjects;
};




/**
 * 
 */
UCLASS()
class TEMPORARY_API UObjectPoolWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()


public:
    UObjectPoolWorldSubsystem();

    // 서브시스템에서는 이게 beginplay
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;


    UPROPERTY()
    FPoolerCleanup OnPoolCleanup;

    // 소환한 액터들을 이름과 매칭해서 가져가기
    UFUNCTION(BlueprintCallable)
    AActor* GetPooledActor(FString Name);

    // 액터를 넘겨주면 회수할 수 있도록 만들어주기
    UFUNCTION(BlueprintCallable)
    void RecycleActor(AActor* PooledActor);

    // 최종 회수
    UFUNCTION(BlueprintCallable)
    void RecyclePooledObject(class UPooledObjectComponent* PoolCompRef);

    // 집나갔던 액터들 전부 회수
    UFUNCTION(BlueprintCallable)
    void Broadcast_PoolerCleanup();

protected:
    // 만들어둔 액터에 붙어있는 컴포넌트를 모아두는 장소
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TArray<FObjectPoolComponentList> Pools;

    // 데이터 에셋
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UObjectPoolDataAsset> PoolConfigAsset;

private:
    // 즉시 만들기
    void RegenItem(int32 PoolIndex, int32 PositionIndex);
};
