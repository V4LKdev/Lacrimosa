// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.


#include "LacrCharacterBase.h"

#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Camera/CameraComponent.h"
#include "CameraStateSystem/CameraManager/CameraStateManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Paper2D/Classes/PaperFlipbookComponent.h"

ALacrCharacterBase::ALacrCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(RootComponent);
}

void ALacrCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	check(PlayerController);
	PlayerController->SetViewTarget(this);

	CameraStateManager = Cast<ACameraStateManager>(PlayerController->PlayerCameraManager);
	check(CameraStateManager);

	CameraStateManager->OnCameraStateChanged.AddUObject(this, &ThisClass::OnCameraStateChanged);

	FCameraStateChange DefaultStateChange;
	DefaultStateChange.CameraStateData = DefaultCameraState;
	DefaultStateChange.Transition = FCameraStateTransition();
	
	CameraStateManager->ChangeCameraState(DefaultStateChange, GetActorLocation());
}

void ALacrCharacterBase::OnCameraStateChanged(const UCameraStateData_Base* CameraState)
{
	// Tiny 4-element LUTs indexed by GameplayAxis (must line up with the enum values)
	static const FVector ForwardLUT[4] = {
		/* X_Right */ FVector(1,  0, 0),
		/* X_Left  */ FVector( -1,  0, 0),
		/* Y_Right */ FVector( 0,  -1, 0),
		/* Y_Left  */ FVector( 0, 1, 0)
	};
	static const FVector RightLUT[4] = {
		/* X_Right */ FVector( 0, -1, 0),
		/* X_Left  */ FVector( 0,  1, 0),
		/* Y_Right */ FVector(-1,  0, 0),
		/* Y_Left  */ FVector( 1,  0, 0)
	};

	const int32 Idx = int32(CameraState->GameplayAxis);
	GameplayAxisForward = ForwardLUT[Idx];
	GameplayAxisRight   = RightLUT[Idx];
}

void ALacrCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsOverridingMovement)
	{
		InputMove(FVector2D::ZeroVector);
	}
	if (bIsAIMoving)
	{
		const FVector2D Velocity2D = FVector2D(GetVelocity().X, GetVelocity().Y);

		// Normalize velocity and project it onto the gameplay axes
		const FVector2D ProjectedDirection = FVector2D(
			FVector2D::DotProduct(Velocity2D, FVector2D(GameplayAxisForward.X, GameplayAxisForward.Y)),
			FVector2D::DotProduct(Velocity2D, FVector2D(GameplayAxisRight.X, GameplayAxisRight.Y))
		).GetSafeNormal();

		AbsoluteCharacterDirection = ProjectedDirection;
		
		
		if (UKismetMathLibrary::Vector_Distance(MoveToTargetLocation, GetActorLocation()) < 100.f && GetVelocity().Length() == 0.f)
		{
			// Stop moving if we are close to the target location
			bIsAIMoving = false;
			GetCharacterMovement()->StopMovementImmediately();
			GetWorldTimerManager().ClearTimer(ForceMoveToTimer);
		}
	}

	// Call visual update tick every X seconds
	static float TimeSinceLastUpdate = 0.f;
	TimeSinceLastUpdate += DeltaTime;
	if (TimeSinceLastUpdate >= SpriteCameraRotationUpdateRate)
	{
		TimeSinceLastUpdate = 0.f;
		UpdateSprite();
	}
}

void ALacrCharacterBase::UpdateSprite()
{
	if (CameraStateManager)
	{
		const auto& CamView = CameraStateManager->GetCameraCacheView();

		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(CamView.Location, GetActorLocation());
		LookAtRot = FRotator(0, LookAtRot.Yaw + 90, 0);

		GetSprite()->SetWorldRotation(LookAtRot);
	}
}

void ALacrCharacterBase::InputMove(const FInputActionValue& Value)
{
	PlayerInputAxis = Value.Get<FVector2D>();

	if (bIsOverridingMovement)
	{
		PlayerInputAxis = OverrideInputAxis;
	}
	
	if (bIsMovementLocked || bIsAIMoving ||PlayerInputAxis.IsZero())
	{
		AbsoluteCharacterDirection = FVector2D::ZeroVector;
		return;
	}

	PlayerInputAxis = PlayerInputAxis.GetSafeNormal();

	AbsoluteCharacterDirection = PlayerInputAxis;


	AddMovementInput(GameplayAxisForward, PlayerInputAxis.X);
	AddMovementInput(GameplayAxisRight, PlayerInputAxis.Y);
}

void ALacrCharacterBase::LockMovementInput(const bool bLockMovement) const
{
	bIsMovementLocked = bLockMovement;
}

void ALacrCharacterBase::ForceMoveTo(const FVector& TargetLocation) const
{
	AController* PC = GetController();
	if (!PC)
	{
		return;
	}
	
	FVector ValidLocation;

	if (UAIBlueprintHelperLibrary::IsValidAILocation(TargetLocation))
	{
		ValidLocation = TargetLocation;
	}
	else
	{
		const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
		if (!NavSys)
		{
			UE_LOG(LogTemp, Warning, TEXT("No Navigation System found"));
			return;
		}

		FNavLocation NavLocation;
		if (NavSys->ProjectPointToNavigation(TargetLocation, NavLocation))
		{
			ValidLocation = NavLocation.Location;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No valid location found for %s"), *TargetLocation.ToString());
			return;
		}
	}

	bIsAIMoving = true;
	bIsOverridingMovement = false;
	
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(PC, ValidLocation);

	MoveToTargetLocation = ValidLocation;

	float TimeoutDuration = 15.0f;
	
	GetWorldTimerManager().SetTimer(
		ForceMoveToTimer,
		[this](){ bIsAIMoving = false; },
		TimeoutDuration,
		false
		);
}

void ALacrCharacterBase::ForceMoveAlong(const EMoveAlongDirection Direction, const float Duration)
{
	bIsOverridingMovement = true;

	// Determine the movement direction based on the enum
	switch (Direction)
	{
	case EMoveAlongDirection::Forward:
		OverrideInputAxis = FVector2D(0.f, -1.f);
		break;
	case EMoveAlongDirection::Backward:
		OverrideInputAxis = FVector2D(0.f, 1.f);
		break;
	case EMoveAlongDirection::Left:
		OverrideInputAxis = FVector2D(-1.f, 0.f);
		break;
	case EMoveAlongDirection::Right:
		OverrideInputAxis = FVector2D(1.f, 0.f);
		break;
	default:
		OverrideInputAxis = FVector2D::ZeroVector;
		break;
	}

	FTimerHandle ForceMoveAlongTimer;
	
	GetWorldTimerManager().SetTimer(
		ForceMoveAlongTimer,
		[this](){ bIsOverridingMovement = false; },
		Duration,
		false
		);
}


