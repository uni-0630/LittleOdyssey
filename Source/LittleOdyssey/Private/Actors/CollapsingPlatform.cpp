// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CollapsingPlatform.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LittleOdysseyCharacter.h"
#include "TimerManager.h"

// Sets default values
ACollapsingPlatform::ACollapsingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 체크포인트의 충돌 감지 영역(BoxCollider) 생성
	mBoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));

	// 체크포인트의 외형을 표시할 메시 컴포넌트 생성
	mBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));

	// BoxCollider를 액터의 루트 컴포넌트로 설정
	// 체크포인트의 위치와 회전 등을 결정하는 기준 컴포넌트
	RootComponent = mBoxCollider;

	// BoxMesh를 BoxCollider의 자식 컴포넌트로 연결
	// 부모인 BoxCollider가 이동하거나 회전하면 BoxMesh도 함께 따라감
	mBoxMesh->SetupAttachment(mBoxCollider);

	// BoxCollider에서 Overlap이 발생하면
	// 현재 Checkpoint 객체의 OnBeginOverlap 함수를 호출하도록 연결
	mBoxCollider->OnComponentBeginOverlap.AddDynamic(
		this, &ACollapsingPlatform::OnComponentBeginOverlap);

	// 다른 컴포넌트와 겹쳤을 때 Overlap 이벤트를 발생시키도록 설정
	mBoxCollider->SetGenerateOverlapEvents(true);

	// BoxCollider를 Trigger 용도로 설정
	// 물체를 물리적으로 막지 않고 겹침을 감지하는 데 사용
	mBoxCollider->SetCollisionProfileName(TEXT("Trigger"));
}

// BoxCollider에 다른 컴포넌트가 들어왔을 때 호출되는 함수
void ACollapsingPlatform::OnComponentBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, // 이벤트가 발생한 컴포넌트
	AActor* OtherActor,                       // 겹친 상대 액터
	UPrimitiveComponent* OtherComp,           // 상대 액터에서 겹친 컴포넌트
	int32 OtherBodyIndex,                     // 상대 컴포넌트의 물리 바디 인덱스
	bool bFromSweep,                          // 이동 중 Sweep 검사로 발생한 이벤트인지
	const FHitResult& SweepResult)            // Sweep 충돌 관련 정보
{
	// Overlap 이벤트가 정상적으로 호출되었는지 로그로 확인
	UE_LOG(LogTemp, Warning, TEXT("CollapsingPlatform!"));

	// 이미 낙하가 예약되었거나 진행 중이면 무시
	if (mIsFalling)
	{
		return;
	}

	mStartLocation = GetActorLocation();

	mCollapseDelay = 0.7f;

	mIsFalling = true;
}

// Called when the game starts or when spawned
void ACollapsingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACollapsingPlatform::RespawnPlatform()
{
	// 발판을 최초 위치로 복구
	SetActorLocation(mStartLocation);

	// 발판 다시 표시
	SetActorHiddenInGame(false);

	// 발판 충돌 다시 활성화
	SetActorEnableCollision(true);

	// 다음에 밟았을 때 다시 0.7초 대기하도록 초기화
	mCollapseDelay = 0.7f;

	// 복구 직후에는 낙하하지 않도록 상태 초기화
	mIsFalling = false;
}

// Called every frame
void ACollapsingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 플레이어가 발판을 밟은 이후에만 타이머 감소
	if (mIsFalling)
	{
		mCollapseDelay -= DeltaTime;

		// 대기 시간이 끝났다면 발판 낙하
		if (mCollapseDelay <= 0.f)
		{
			// 매 프레임 아래쪽으로 이동할 거리 계산
			FVector DeltaLocation = FVector(0.f, 0.f, -mFallSpeed * DeltaTime);

			// 현재 위치를 기준으로 발판 이동
			AddActorWorldOffset(DeltaLocation);
		}

		// 발판이 최초 위치에서 아래로 1000cm 이상 떨어졌는지 확인
		if (GetActorLocation().Z <= mStartLocation.Z - 1000.f)
		{
			// 발판 낙하 중지
			mIsFalling = false;

			// 발판 숨기기
			SetActorHiddenInGame(true);

			// 충돌 비활성화
			SetActorEnableCollision(false);

			// 3초 후 RespawnItem 함수를 한 번 실행하도록 예약
			GetWorldTimerManager().SetTimer(
				mRespawnTimerHandle,
				this,
				&ACollapsingPlatform::RespawnPlatform,
				mRespawnDelay,
				false
			);
		}
	}
}

