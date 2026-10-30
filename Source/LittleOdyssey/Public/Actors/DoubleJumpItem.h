// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoubleJumpItem.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class LITTLEODYSSEY_API ADoubleJumpItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADoubleJumpItem();

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* mBoxCollider;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* mBoxMesh;

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

private:
	// 아이템 재생성까지 대기하는 시간
	UPROPERTY(EditAnywhere, Category = "Item")
	float mRespawnDelay = 3.f;

	// 아이템 재활성화 타이머 핸들
	FTimerHandle mRespawnTimerHandle;

	// 아이템을 다시 활성화하는 함수
	void RespawnItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
