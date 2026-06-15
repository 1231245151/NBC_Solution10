// Fill out your copyright notice in the Description page of Project Settings.


#include "PooledObjectComponent.h"

#include "ObjectPoolWorldSubsystem.h"

void UPooledObjectComponent::Init(UObjectPoolWorldSubsystem* Owner)
{
    bIsPoolActive = false;

    // 오브젝트 풀의 주인
    ObjectPool = Owner;

}

void UPooledObjectComponent::RecycleSelf()
{
    // 예시, 총알 맞췃을 시 호출시킴
    ObjectPool->RecyclePooledObject(this);

}

void UPooledObjectComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    // 해당 컴포넌트가 삭제되었을 시
    ObjectPool->OnPoolCleanup.RemoveDynamic(this, &UPooledObjectComponent::RecycleSelf);
    Super::OnComponentDestroyed(bDestroyingHierarchy);

}
