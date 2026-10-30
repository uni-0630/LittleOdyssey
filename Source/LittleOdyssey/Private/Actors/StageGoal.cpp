// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/StageGoal.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LittleOdysseyCharacter.h"

// Sets default values
AStageGoal::AStageGoal()
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
		this, &AStageGoal::OnBeginOverlap);

	// 다른 컴포넌트와 겹쳤을 때 Overlap 이벤트를 발생시키도록 설정
	mBoxCollider->SetGenerateOverlapEvents(true);

	// BoxCollider를 Trigger 용도로 설정
	// 물체를 물리적으로 막지 않고 겹침을 감지하는 데 사용
	mBoxCollider->SetCollisionProfileName(TEXT("Trigger"));
}

// BoxCollider에 다른 컴포넌트가 들어왔을 때 호출되는 함수
void AStageGoal::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, // 이벤트가 발생한 컴포넌트
	AActor* OtherActor,                       // 겹친 상대 액터
	UPrimitiveComponent* OtherComp,           // 상대 액터에서 겹친 컴포넌트
	int32 OtherBodyIndex,                     // 상대 컴포넌트의 물리 바디 인덱스
	bool bFromSweep,                          // 이동 중 Sweep 검사로 발생한 이벤트인지
	const FHitResult& SweepResult)            // Sweep 충돌 관련 정보
{
	// Overlap 이벤트가 정상적으로 호출되었는지 로그로 확인
	UE_LOG(LogTemp, Warning, TEXT("Stage Change!"));

	// 체크포인트 때처럼 플레이어인지 확인
	ALittleOdysseyCharacter* Player =
		Cast<ALittleOdysseyCharacter>(OtherActor);

	if (Player)
	{
		// 에디터에서 설정한 다음 레벨로 이동
		UGameplayStatics::OpenLevel(this, mNextLevelName);
	}
}

// Called when the game starts or when spawned
void AStageGoal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStageGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

