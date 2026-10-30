// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class LITTLEODYSSEY_API AMovingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingPlatform();

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* mBoxCollider;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* mBoxMesh;

	// 발판의 시작 위치
	UPROPERTY(EditAnywhere, Category = "Moving Platform")
	FVector mStartLocation;
	
	// 발판의 도착 위치
	UPROPERTY(EditAnywhere, Category = "Moving Platform")
	FVector mEndLocation;

	// 발판의 상대적인 이동 거리
	UPROPERTY(EditAnywhere, Category = "Moving Platform")
	FVector mMoveOffset = FVector(500.f, 0.f, 0.f);

	// 발판 이동 속도 (cm/s)
	UPROPERTY(EditAnywhere, Category = "Moving Platform")
	float mMoveSpeed = 200.f;

	// 도착 지점에서 기다리는 시간 (초)
	UPROPERTY(EditAnywhere, Category = "Moving Platform")
	float mWaitTime = 0.f;

	// true면 도착 지점으로, false면 시작 지점으로 이동
	bool mMovingToEnd = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
