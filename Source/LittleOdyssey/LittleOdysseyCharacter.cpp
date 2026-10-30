// Copyright Epic Games, Inc. All Rights Reserved.

#include "LittleOdysseyCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "LittleOdyssey.h"

ALittleOdysseyCharacter::ALittleOdysseyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// 게임 시작 시 캐릭터의 기본 이동 설정 저장
	mOriginalGroundFriction =
		GetCharacterMovement()->GroundFriction;

	// 게임 시작 시 캐릭터의 기본 감속력 저장
	mOriginalBrakingDeceleration =
		GetCharacterMovement()->BrakingDecelerationWalking;

	// 게임 시작 시 기본 가속도 저장
	mOriginalMaxAcceleration =
		GetCharacterMovement()->MaxAcceleration;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ALittleOdysseyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALittleOdysseyCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ALittleOdysseyCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALittleOdysseyCharacter::Look);
	}
	else
	{
		UE_LOG(LogLittleOdyssey, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ALittleOdysseyCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ALittleOdysseyCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ALittleOdysseyCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ALittleOdysseyCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ALittleOdysseyCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ALittleOdysseyCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

// 게임 시작 시 캐릭터의 초기 위치 저장
void ALittleOdysseyCharacter::BeginPlay()
{
	Super::BeginPlay();

	mStartLocation = GetActorLocation();
}

// 매 프레임 캐릭터의 높이를 확인하여 낙사 판정
void ALittleOdysseyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 지정된 높이보다 아래로 떨어지면 리스폰
	if (GetActorLocation().Z < mFallLimitZ)
	{
		Respawn();
	}

	// 코요테 타임이 남아 있을 때만 감소
	if (mCoyoteTimer > 0.f)
	{
		mCoyoteTimer -= DeltaTime;

		// 코요테 타임이 만료되었다면
		if (mCoyoteTimer <= 0.f)
		{
			// 코요테 타임 만료 시점의 점프 상태 확인
			UE_LOG(LogTemp, Warning,
				TEXT("Coyote Expired | JumpCurrent: %d | JumpMax: %d | ItemTimer: %.2f"),
				JumpCurrentCount,
				JumpMaxCount,
				mDoubleJumpTimer);

			// 타이머 초기화
			mCoyoteTimer = 0.f;

			// 사용 가능한 아이템 능력에 따라 점프 제한 복구
			JumpMaxCount = (mDoubleJumpTimer > 0.f) ? 2 : 1;
		}
	}

	// 더블 점프 능력의 남은 시간이 있을 때만 처리
	if (mDoubleJumpTimer > 0.f)
	{
		// 매 프레임 남은 시간 감소
		mDoubleJumpTimer -= DeltaTime;

		// 지속 시간이 만료되면 능력 종료
		if (mDoubleJumpTimer <= 0.f)
		{
			DeactivateDoubleJump();
		}
	}
}

// 캐릭터를 초기 위치로 이동시키고 이동 속도 초기화
void ALittleOdysseyCharacter::Respawn()
{
	// 초기 스폰 위치로 이동
	SetActorLocation(mStartLocation);

	// 낙하 속도 및 현재 이동 속도 제거
	GetCharacterMovement()->StopMovementImmediately();
}

// 캐릭터의 리스폰 위치를 변경
void ALittleOdysseyCharacter::SetRespawnLocation(FVector NewLocation)
{
	// 시작 위치를 리스폰 위치로 변경
	mStartLocation = NewLocation;
}

// 캐릭터의 리스폰 위치 번호를 설정
bool ALittleOdysseyCharacter::SetCheckpointIndex(int32 Index)
{
	// 현재 체크포인트 번호가 다음 체크포인트 번호보다 낮다면
	if (mCurrentCheckpointIndex < Index)
	{
		// 체크포인트 번호를 변경
		mCurrentCheckpointIndex = Index;

		return true;
	}

	return false;
}

void ALittleOdysseyCharacter::OnMovementModeChanged(
	EMovementMode PrevMovementMode,
	uint8 PreviousCustomMode)
{
	// 부모 클래스의 이동 상태 변경 처리 실행
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	// 이전 이동 상태가 걷기였는지 확인
	if (PrevMovementMode == MOVE_Walking)
	{
		if (GetCharacterMovement()->MovementMode == MOVE_Falling)
		{
			// 걷기 → 낙하 상태로 변경된 순간!	
			UE_LOG(LogTemp, Warning, TEXT("Walking -> Falling"));
		}
	}
}

// 더블 점프 아이템 효과 활성화
void ALittleOdysseyCharacter::ActivateDoubleJump()
{
	// 코요테 점프를 사용했다면 점프 횟수 보정을 유지
	// 일반 점프 상태라면 기본 더블 점프 횟수인 2로 설정
	JumpMaxCount = mUsedCoyoteJump ? 3 : 2;

	// 능력의 남은 시간을 1.5초로 설정
	mDoubleJumpTimer = 1.5f;
}

// 더블 점프 아이템 효과 종료
void ALittleOdysseyCharacter::DeactivateDoubleJump()
{
	// 아이템 지속 시간 초기화
	mDoubleJumpTimer = 0.f;

	// 코요테 점프를 아직 사용하지 않았다면 임시 점프 2회 유지
	// 아니라면 일반 점프 1회로 복구
	JumpMaxCount = (mCoyoteTimer > 0.f && !mHasJumped) ? 2 : 1;
}

// 얼음 바닥에 진입했을 때 호출
void ALittleOdysseyCharacter::ApplyIceMovement()
{
	// 마찰력을 낮춰 방향 전환이 느려지도록 설정
	GetCharacterMovement()->GroundFriction = 0.1f;

	// 감속력을 낮춰 이동을 멈춰도 미끄러지도록 설정
	GetCharacterMovement()->BrakingDecelerationWalking = 1.f;

	// 얼음 위에서는 천천히 가속하도록 설정
	GetCharacterMovement()->MaxAcceleration = 600.f;
}

// 얼음 바닥에서 벗어났을 때 호출
void ALittleOdysseyCharacter::RestoreMovementSettings()
{
	// 저장했던 기본 마찰력으로 복구
	GetCharacterMovement()->GroundFriction = mOriginalGroundFriction;

	// 저장했던 기본 감속력으로 복구
	GetCharacterMovement()->BrakingDecelerationWalking =
		mOriginalBrakingDeceleration;

	// 얼음 바닥에서 벗어나면 원래 가속도로 복구
	GetCharacterMovement()->MaxAcceleration = mOriginalMaxAcceleration;
}

void ALittleOdysseyCharacter::OnJumped_Implementation()
{
	// 부모 클래스의 기본 점프 처리
	Super::OnJumped_Implementation();

	// 점프가 성공했을 때 현재 점프 횟수와 최대 허용 횟수 확인
	UE_LOG(LogTemp, Warning,
		TEXT("JumpCurrent: %d | JumpMax: %d | Coyote: %d | ItemTimer: %.2f"),
		JumpCurrentCount,
		JumpMaxCount,
		mUsedCoyoteJump ? 1 : 0,
		mDoubleJumpTimer);

	// 코요테 타임 중 첫 점프인지 확인
	if (mCoyoteTimer > 0.f && !mHasJumped)
	{
		// 코요테 점프 사용 기록
		mUsedCoyoteJump = true;

		// 코요테 점프는 이미 카운트 2를 사용했으므로
		// 아이템이 있을 때만 세 번째 점프를 허용
		JumpMaxCount = (mDoubleJumpTimer > 0.f) ? 3 : 1;
	}
	// 아이템 효과가 활성화된 상태에서 추가 점프를 사용했는지 확인
	else if (mDoubleJumpTimer > 0.f)
	{
		// 코요테 점프를 사용했으면 세 번째 점프가 추가 점프
		// 일반 점프라면 두 번째 점프가 추가 점프
		const int32 ExtraJumpCount = mUsedCoyoteJump ? 3 : 2;

		if (JumpCurrentCount == ExtraJumpCount)
		{
			// 아이템 효과 종료
			DeactivateDoubleJump();
		}
	}

	// 점프 사용 여부 기록
	mHasJumped = true;

	// 코요테 타임 초기화
	mCoyoteTimer = 0.f;
}

// 캐릭터가 점프하지 않고 발판 끝에서 걸어 떨어졌을 때 호출
void ALittleOdysseyCharacter::OnWalkingOffLedge_Implementation(
	const FVector& PreviousFloorImpactNormal,
	const FVector& PreviousFloorContactNormal,
	const FVector& PreviousLocation,
	float TimeDelta)
{
	// 발판에서 벗어나는 이벤트가 발생했는지 확인
	UE_LOG(LogTemp, Warning, TEXT("Walking Off Ledge!"));

	// 발판에서 떨어지면 코요테 타임 시작
	mCoyoteTimer = 0.15f;

	// 발판에서 떨어지며 소비한 점프 카운트 1회를 보정
	// 아이템이 있으면 최대 3회, 없으면 최대 2회로 설정
	JumpMaxCount = (mDoubleJumpTimer > 0.f) ? 3 : 2;
}

// 기본 점프 조건에 코요테 타임 조건 추가
bool ALittleOdysseyCharacter::CanJumpInternal_Implementation() const
{
	// 언리얼 기본 점프 가능 여부 확인
	const bool bCanJump = Super::CanJumpInternal_Implementation();

	// 기본 점프가 가능하거나 코요테 타임이 남아 있으면 true 반환
	return bCanJump || mCoyoteTimer > 0.f;
}

// 캐릭터가 바닥에 착지했을 때 호출
void ALittleOdysseyCharacter::Landed(const FHitResult& Hit)
{
	// 부모 클래스의 착지 처리 실행
	Super::Landed(Hit);

	// 새로운 점프를 할 수 있도록 상태 초기화
	mHasJumped = false;

	// 이전 코요테 점프 사용 기록 초기화
	mUsedCoyoteJump = false;

	// 코요테 타이머 초기화
	mCoyoteTimer = 0.f;
}