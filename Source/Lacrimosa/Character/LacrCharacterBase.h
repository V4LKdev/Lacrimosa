// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Lacrimosa/Interface/InputReceiverInterface.h"
#include "PaperZD/Public/PaperZDCharacter.h"
#include "LacrCharacterBase.generated.h"

class ACameraStateManager;
class UCameraStateData_Base;
class UCameraComponent;

UENUM(BlueprintType)
enum class EMoveAlongDirection : uint8
{
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right"),
	Forward		UMETA(DisplayName = "Forward"),
	Backward	UMETA(DisplayName = "Backward"),
};


UCLASS(Abstract)
class LACRIMOSA_API ALacrCharacterBase : public APaperZDCharacter, public IInputReceiverInterface
{
	GENERATED_BODY()
	
public:
	ALacrCharacterBase();
	virtual void Tick(float DeltaTime) override;

	/** IInputReceiver interface */
	virtual void InputMove(const FInputActionValue& Value) override;
	virtual void InputInteract(const FInputActionValue& Value) override {}
	/** End of IInputReceiver interface */

	UFUNCTION(BlueprintCallable)
	void LockMovementInput(bool bLockMovement) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	void ForceMoveTo(const FVector& TargetLocation) const;
	UFUNCTION(BlueprintCallable)
	void ForceMoveAlong(const EMoveAlongDirection Direction, float Duration);

	FVector2D AbsoluteCharacterDirection = FVector2D::ZeroVector;

protected:
	virtual void BeginPlay() override;
	virtual void UpdateSprite();
	
	void OnCameraStateChanged(const UCameraStateData_Base* CameraState);
	
	FVector GameplayAxisForward = FVector::ZeroVector;
	FVector GameplayAxisRight = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	const TObjectPtr<UCameraStateData_Base> DefaultCameraState;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCameraComponent> PlayerCamera;

	UPROPERTY()
	TObjectPtr<ACameraStateManager> CameraStateManager;

	UPROPERTY(EditAnywhere)
	float SpriteCameraRotationUpdateRate = 0.25f;

private:
	
	FVector2D PlayerInputAxis = FVector2D::ZeroVector;

	mutable bool bIsMovementLocked = false;
	
	mutable bool bIsOverridingMovement = false;
	FVector2D OverrideInputAxis = FVector2D::ZeroVector;

	mutable bool bIsAIMoving = false;
	mutable FTimerHandle ForceMoveToTimer;

	mutable FVector MoveToTargetLocation = FVector::ZeroVector;
};