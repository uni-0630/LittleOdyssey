// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "LittleOdysseyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class ALittleOdysseyCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MouseLookAction;

public:

	/** Constructor */
	ALittleOdysseyCharacter();

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:

	// 캐릭터의 초기 스폰 위치
	FVector mStartLocation;

	// 낙사 판정 기준 높이 (월드 Z 좌표)
	UPROPERTY(EditAnywhere, Category = "Respawn")
	float mFallLimitZ = -500.f;

	// 캐릭터를 저장된 위치로 이동시키는 리스폰 함수
	void Respawn();

	// 플레이어가 현재 도달한 가장 높은 체크포인트 번호
	int32 mCurrentCheckpointIndex = 0;

private:
	// 일반 점프를 했는지 구분하기 위한 변수
	bool mHasJumped = false;

	// 발판에서 떨어진 후 코요테 타임의 남은 시간
	float mCoyoteTimer = 0.f;

	// 능력의 남은 시간 설정
	float mDoubleJumpTimer = 1.5f;

	// 이번 공중 이동에서 코요테 점프를 사용했는지 기록
	bool mUsedCoyoteJump = false;

	// 얼음 바닥에 진입하기 전의 마찰력
	float mOriginalGroundFriction = 0.f;

	// 얼음 바닥에 진입하기 전의 감속력
	float mOriginalBrakingDeceleration = 0.f;

	// 캐릭터의 원래 가속도 저장
	float mOriginalMaxAcceleration = 0.f;

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:

	// 매 프레임 낙사 여부 확인
	virtual void Tick(float DeltaTime) override;

	// 게임 시작 시 초기 위치 저장
	virtual void BeginPlay() override;

	// 리스폰 위치를 변경
	void SetRespawnLocation(FVector NewLocation);

	// 리스폰 번호를 변경
	bool SetCheckpointIndex(int32 Index);

	// 캐릭터의 이동 모드가 변경되었을 때 호출되는 함수
	virtual void OnMovementModeChanged(
		EMovementMode PrevMovementMode,
		uint8 PreviousCustomMode = 0
	) override;

	// 더블 점프 아이템 효과를 활성화하는 함수
	void ActivateDoubleJump();

	// 더블 점프 아이템 효과를 비활성화하는 함수
	void DeactivateDoubleJump();

	// 얼음 바닥의 미끄러지는 이동 효과 적용
	void ApplyIceMovement();

	// 일반 바닥의 이동 설정으로 복구
	void RestoreMovementSettings();

protected:
	// 캐릭터가 실제로 점프했을 때 호출되는 함수
	virtual void OnJumped_Implementation() override;

	// 캐릭터가 걷다가 발판 끝에서 벗어났을 때 호출되는 이벤트 구현 함수
	virtual void OnWalkingOffLedge_Implementation(
		const FVector& PreviousFloorImpactNormal,
		const FVector& PreviousFloorContactNormal,
		const FVector& PreviousLocation,
		float TimeDelta
	) override;

	// 캐릭터가 현재 점프 가능한 상태인지 판단
	virtual bool CanJumpInternal_Implementation() const override;

	// 캐릭터가 바닥에 착지했을 때 호출
	virtual void Landed(const FHitResult& Hit) override;
};