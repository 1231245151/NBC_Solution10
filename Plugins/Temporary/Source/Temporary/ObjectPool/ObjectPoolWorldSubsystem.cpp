// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPoolWorldSubsystem.h"

#include "PooledObjectComponent.h"

UObjectPoolWorldSubsystem::UObjectPoolWorldSubsystem()
{
    FString RefText = FString::Printf(TEXT("/Script/Temporary.ObjectPoolDataAsset'/Temporary/DA_ActorSpawn.DA_ActorSpawn'"));
    static ConstructorHelpers::FObjectFinder<UObjectPoolDataAsset> DataAsset(*RefText);

    if (DataAsset.Succeeded())
    {
        // 내 포인터에 찰싹 연결!
        PoolConfigAsset = DataAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("데이터 에셋 경로를 찾을 수 없습니다! 경로를 확인해주세요."));
    }
}

void UObjectPoolWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    // 에디터에서 데이터 에셋을 안 넣었으면 작동 방지
    if (!PoolConfigAsset)
        return;

    FActorSpawnParameters SpawnParams;


    for (int32 PoolIndex = 0; PoolIndex < PoolConfigAsset->PoolDataArray.Num(); ++PoolIndex)
    {
        FObjectPoolComponentList CurrentpoolIndex;

        FPooledObjectData& CurrentData = PoolConfigAsset->PoolDataArray[PoolIndex];

        SpawnParams.Name = FName(FString::Printf(TEXT("%s"), *CurrentData.ActorName));
        SpawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        for (int32 ObjectIndex = 0; ObjectIndex < CurrentData.Poolsize; ++ObjectIndex)
        {
            AActor* SpawnedActor = GetWorld()->SpawnActor(CurrentData.ActorTemplate, &FVector::ZeroVector, &FRotator::ZeroRotator, SpawnParams);
            SpawnedActor->SetActorLabel(SpawnedActor->GetName());

            UPooledObjectComponent* PoolComp = NewObject<UPooledObjectComponent>(SpawnedActor);
            PoolComp->RegisterComponent();
            SpawnedActor->AddInstanceComponent(PoolComp);

            PoolComp->Init(this);

            CurrentpoolIndex.PooledObjects.Add(PoolComp);

            SpawnedActor->SetActorHiddenInGame(true);
            SpawnedActor->SetActorEnableCollision(false);
            SpawnedActor->SetActorTickEnabled(false);


            //SpawnedActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        }
        Pools.Add(CurrentpoolIndex);
    }
}

AActor* UObjectPoolWorldSubsystem::GetPooledActor(FString Name)
{
    // 우리가 저장한 컴포넌트의 갯수를 받아준다.
    int32 PoolCount = Pools.Num();

    // 원하는 액터가 몇번째 인덱스에서 담겨잇나
    int32 CurrentPoolIndex = -1;

    for (int32 i = 0; i < PoolCount; ++i)
    {
        // 찾는 이름과 같다면
        if (PoolConfigAsset->PoolDataArray[i].ActorName == Name)
        {
            CurrentPoolIndex = i;
            break;
        }
    }

    // 존재하지않으면 나가기
    if (CurrentPoolIndex == -1)
        return nullptr;

    // 해당 인덱스가 들어있는 컴포넌트 개수
    int32 PoolObjectCount = Pools[CurrentPoolIndex].PooledObjects.Num();

    // 여기 안에서도 어디부터 사용할수 있는지 확인
    int32 FirstAvailable = -1;


    // 순회
    // 찾은 인덱스 안에서  어디부터 사용중인지 판단
    for (int32 i = 0; i < PoolObjectCount; ++i)
    {
        // 안에 내용물(컴포넌트)이 nullptr이 아닐시
        if (Pools[CurrentPoolIndex].PooledObjects[i] != nullptr)
        {
            // 그 컴포넌트가 Active가 아닐 시
            if (!Pools[CurrentPoolIndex].PooledObjects[i]->bIsPoolActive)
            {
                FirstAvailable = i;
                break;
            }
        }
        // 여기왔다? 그놈은 뭔가잘못만들어진놈이니 재생성하라
        else
        {
            // 재생성
            RegenItem(CurrentPoolIndex, i);

            // 이제 잘된 데이터일거니 이거써라
            FirstAvailable = i;
            break;
        }
    }

    // 사용가능한 컴포넌트가 있다.
    if (FirstAvailable >= 0)
    {
        // 만들어진 오브젝트의 컴포넌트를 끌고와서 사용중으로 변경
        UPooledObjectComponent* ToReturn = Pools[CurrentPoolIndex].PooledObjects[FirstAvailable];
        ToReturn->bIsPoolActive = true;

        // 회수 예약, 언제 어디서 회수할지모르기에 일단 예약
        OnPoolCleanup.AddUniqueDynamic(ToReturn, &UPooledObjectComponent::RecycleSelf);

        // 활성화 작업
        AActor* ToReturnActor = ToReturn->GetOwner();
        ToReturnActor->SetActorHiddenInGame(false);
        ToReturnActor->SetActorTickEnabled(true);
        ToReturnActor->SetActorEnableCollision(true);

        // 부모를 끊고 최상단으로 간다.
        ToReturnActor->AttachToActor(nullptr, FAttachmentTransformRules::SnapToTargetIncludingScale);


        return ToReturnActor;
    }

    // 사용가능한 컴포넌트가 없다.
    // 추가생성이 가능한옵션이 켜져있지 않으면 종료.
    if (!PoolConfigAsset->PoolDataArray[CurrentPoolIndex].bCanGrow)
        return nullptr;

    // bCanGrow가 켜져있음
    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = FName(FString::Printf(TEXT("%s"), *PoolConfigAsset->PoolDataArray[CurrentPoolIndex].ActorName));
    SpawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    FObjectPoolComponentList CurrentpoolIndex;

    // 스폰
    AActor* SpawnedActor = GetWorld()->SpawnActor(PoolConfigAsset->PoolDataArray[CurrentPoolIndex].ActorTemplate, &FVector::ZeroVector, &FRotator::ZeroRotator, SpawnParams);
    // 아웃라이너에서 뜨는 이름 재설정
    SpawnedActor->SetActorLabel(SpawnedActor->GetName());
    // 컴포넌트 만들기
    UPooledObjectComponent* PoolComp = NewObject<UPooledObjectComponent>(SpawnedActor);
    // 컴포넌트를 언리얼에 기능적 등록
    PoolComp->RegisterComponent();
    // 특정 액터의 소유다
    SpawnedActor->AddInstanceComponent(PoolComp);
    // PooledObject의 초기화 함수 호출
    PoolComp->Init(this);
    // 방금 만들어준 컴포넌트를 저장 중
    CurrentpoolIndex.PooledObjects.Add(PoolComp);

    //사용중 처리
    PoolComp->bIsPoolActive = true;

    OnPoolCleanup.AddUniqueDynamic(PoolComp, &UPooledObjectComponent::RecycleSelf);

    return SpawnedActor;
}

void UObjectPoolWorldSubsystem::RecycleActor(AActor* PooledActor)
{
    if (UPooledObjectComponent* PoolCompRef = Cast<UPooledObjectComponent>(PooledActor->GetComponentByClass(UPooledObjectComponent::StaticClass())))
    {
        RecyclePooledObject(PoolCompRef);
    }

}

void UObjectPoolWorldSubsystem::RecyclePooledObject(UPooledObjectComponent* PoolCompRef)
{
    // 바인딩 회수
    OnPoolCleanup.RemoveDynamic(PoolCompRef, &UPooledObjectComponent::RecycleSelf);

    // 비 사용중으로 바꾸기
    PoolCompRef->bIsPoolActive = false;

    // 안보이게 만들기
    AActor* ReturnningActor = PoolCompRef->GetOwner();
    ReturnningActor->SetActorHiddenInGame(true);
    ReturnningActor->SetActorEnableCollision(false);
    ReturnningActor->SetActorTickEnabled(false);
    //ReturnningActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);


}

void UObjectPoolWorldSubsystem::Broadcast_PoolerCleanup()
{
    // 전부지우기용 신호보냄!
    OnPoolCleanup.Broadcast();
}

// 급히 만들기
void UObjectPoolWorldSubsystem::RegenItem(int32 PoolIndex, int32 PositionIndex)
{
    //스폰 준비
    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = FName(FString::Printf(TEXT("%s"), *PoolConfigAsset->PoolDataArray[PoolIndex].ActorName));
    SpawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 스폰
    AActor* SpawnedActor = GetWorld()->SpawnActor(PoolConfigAsset->PoolDataArray[PoolIndex].ActorTemplate, &FVector::ZeroVector, &FRotator::ZeroRotator, SpawnParams);
    // 아웃라이너에서 뜨는 이름 재설정
    SpawnedActor->SetActorLabel(SpawnedActor->GetName());

    UPooledObjectComponent* PoolComp = NewObject<UPooledObjectComponent>(SpawnedActor);
    PoolComp->RegisterComponent();
    SpawnedActor->AddInstanceComponent(PoolComp);

    PoolComp->Init(this);

    Pools[PoolIndex].PooledObjects.Insert(PoolComp, PositionIndex);

    SpawnedActor->SetActorHiddenInGame(true);
    SpawnedActor->SetActorEnableCollision(false);
    SpawnedActor->SetActorTickEnabled(false);
    //SpawnedActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}
