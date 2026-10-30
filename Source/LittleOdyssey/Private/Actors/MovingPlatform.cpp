// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/MovingPlatform.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
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

	// 다른 컴포넌트와 겹쳤을 때 Overlap 이벤트를 발생시키도록 설정
	mBoxCollider->SetGenerateOverlapEvents(true);

	// BoxCollider를 Trigger 용도로 설정
	// 물체를 물리적으로 막지 않고 겹침을 감지하는 데 사용
	mBoxCollider->SetCollisionProfileName(TEXT("Trigger"));
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	// 발판의 시작 위치 저장
	mStartLocation = GetActorLocation();

	// 시작 위치에 상대 이동 거리를 더해 도착 위치 계산
	mEndLocation = mStartLocation + mMoveOffset;
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 이동 방향에 따라 목표 위치 결정
	FVector TargetLocation = mMovingToEnd
		? mEndLocation
		: mStartLocation;

	// 목표 위치를 향해 일정한 속도로 이동
	FVector NewLocation = FMath::VInterpConstantTo(
		GetActorLocation(),
		TargetLocation,
		DeltaTime,
		mMoveSpeed
	);

	// 발판을 계산된 위치로 이동
	SetActorLocation(NewLocation);

	// 목표 위치에 도착했는지 확인
	if (GetActorLocation().Equals(TargetLocation, 1.f))
	{
		// 이동 방향 반전
		mMovingToEnd = !mMovingToEnd;
	}
}

