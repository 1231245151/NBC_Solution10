## 1. 프로젝트 개요
- **목표**: 새로운 모듈 및 독립적인 플러그인을 직접 생성하고, 상호작용 및 로딩 순서를 제어하여 엔진 아키텍처를 이해합니다.
- **핵심 구현 사항**:
  - `Test` 모듈 수동 생성 및 프로젝트 등록
  - `Temporary` 플러그인 제작 및 환경 설정
  - 모듈 간 상호작용 (Actor Spawning)
  - 도전 과제: 플러그인을 활용한 **오브젝트 풀(Object Pool)** 시스템 구현

---

## 2. 모듈(Test) 생성 및 설정

### 2.1 폴더 구조 및 파일 생성
`Source/Test` 폴더를 생성하고 모듈의 기본 구성 요소인 `Build.cs`, `.h`, `.cpp` 파일을 수동으로 작성하였습니다.

![직접 작성한 Test 모듈 파일](E:/프로젝트용/1.png)

### 2.2 종속성 및 매크로 설정
`Test.Build.cs`에서 `Core`, `Engine`, `CoreUObject` 등 필수 모듈을 추가하여 의존성을 해결하였습니다. `Test.cpp`에서는 `IMPLEMENT_MODULE` 매크로를 통해 모듈을 엔진에 선언했습니다.

![Test.Build.cs 설정](E:/프로젝트용/2.png)

### 2.3 프로젝트 타겟 및 디스크립터 등록
생성한 모듈이 빌드 프로세스에 포함되도록 `Target.cs` 파일들의 `ExtraModuleNames`에 `"Test"`를 추가하고, `.uproject` 파일의 `Modules` 리스트에 등록하였습니다.

![Target.cs 모듈 추가](E:/프로젝트용/4.png)
![uproject 모듈 등록](E:/프로젝트용/5.png)

---

## 3. 플러그인(Temporary) 구축

### 3.1 플러그인 기본 구조
`Plugins/Temporary` 경로에 `Content`, `Source` 폴더와 플러그인 메타데이터를 담은 `.uplugin` 파일을 구성하였습니다.

![플러그인 디스크립터 참고 자료](E:/프로젝트용/6.png)

### 3.2 LoadingPhase 설정 및 문제 해결
플러그인 내 모듈의 `LoadingPhase`를 `PreDefault`로 설정하여 엔진의 기본 로드 과정보다 앞서 로드되도록 하였습니다.

![LoadingPhase PreDefault 설정](E:/프로젝트용/7.png)

**⚠️ 주의사항 (Troubleshooting)**
플러그인이나 모듈의 `LoadingPhase`를 `Default`로 설정할 경우, 엔진 로드 시 해당 플러그인의 기능을 참조하는 블루프린트나 시스템이 플러그인을 미처 읽지 못해 **"Invalid Subsystem Type"** 또는 노드 오류가 발생할 수 있습니다. 이를 방지하기 위해 로딩 우선순위를 적절히 조절하는 것이 중요합니다.

![LoadingPhase 관련 블루프린트 오류 예시](E:/프로젝트용/8.png)

---

## 4. 모듈 간 상호작용 (TestActor)

`Test` 모듈에서 정의된 `ATestActor`를 주 게임 모듈(`NBC_Solution10`)의 캐릭터 클래스에서 참조하여 `SpawnActor` 로직을 구현했습니다. 이를 통해 서로 다른 모듈 간의 코드 참조와 링크 과정이 정상적으로 이루어짐을 확인했습니다.

---

## 5. 도전 과제: 오브젝트 풀(Object Pool) 시스템

`Temporary` 플러그인을 활용하여 재사용 가능한 **오브젝트 풀링 시스템**을 구현했습니다.

- **주요 구성 요소**:
  - `ObjectPoolWorldSubsystem`: 월드 단위에서 풀링 로직을 관리하는 서브시스템
  - `PooledObjectComponent`: 풀링 대상 액터에 부착하여 상태(Active/Inactive)를 관리하는 컴포넌트
  - `ObjectPoolDataAsset`: 풀의 크기와 생성할 액터 클래스를 설정하는 데이터 에셋
  - `PooledObjectData`: UObject 기반으로 속성 데이터를 저장하고 전달하는 컨테이너
