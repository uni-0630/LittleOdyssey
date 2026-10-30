// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollapsingPlatform.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class LITTLEODYSSEY_API ACollapsingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollapsingPlatform();

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* mBoxCollider;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* mBoxMesh;

	UFUNCTION()
	void OnComponentBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

private:
	// 발판의 최초 위치
	FVector mStartLocation;

	// 발판이 떨어지고 있는지 확인
	bool mIsFalling = false;

	// 발판이 떨어지는 속도 (cm/s)
	float mFallSpeed = 3000.f;

	// 발판이 떨어지기 전 대기 시간
	float mCollapseDelay = 0.7f;

	// 발판 재생성 대기 시간
	float mRespawnDelay = 3.f;

	// 발판 재활성화 타이머 핸들
	FTimerHandle mRespawnTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void RespawnPlatform();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
