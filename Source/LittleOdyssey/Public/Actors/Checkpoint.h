// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USoundBase;

UCLASS()
class LITTLEODYSSEY_API ACheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint();

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* mBoxCollider;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* mBoxMesh;

	// 체크포인트 활성화 효과음
	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundBase> mCheckpointSound = nullptr;

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// 캐릭터가 리스폰할 위치
	FVector mStartLocation;

	// 이 체크포인트의 고유 순서 번호
	// 에디터에서 각 체크포인트마다 번호를 설정할 수 있도록 함
	UPROPERTY(EditAnywhere, Category = "Checkpoint")
	int32 mCheckpointIndex = 1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	

	// 체크포인트가 활성화되었을 때 블루프린트에서 실행할 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Checkpoint")
	void OnCheckpointActivated();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
