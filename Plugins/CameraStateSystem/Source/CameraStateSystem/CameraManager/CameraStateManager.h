// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Camera/PlayerCameraManager.h"
#include "CameraStateSystem/CameraState/CameraStateInterface.h"
#include "CameraStateSystem/CameraState/Data/CameraStateData_Base.h"
#include "CameraStateSystem/Utility/StateCamStructs.h"
#include "Components/TimelineComponent.h"
#include "CameraStateManager.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FCameraStateChangeSignature, const UCameraStateData_Base*);

/**
 *	ACameraStateManager
 *
 *	Manages the state of the camera, including transitions, overrides, and updates.
 */
// TODO: Test Transition spam
UCLASS()
class CAMERASTATESYSTEM_API ACameraStateManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	explicit ACameraStateManager(const FObjectInitializer& ObjectInitializer);
	
	/** Updates the view target after each frame. */
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

	/** Changes the camera state with a specified transition.
	 * Should be called from a GameplayTrigger Subsystem that also handles other classes that depend on this camera change
	 */
	void ChangeCameraState(const FCameraStateChange& NewState, const FVector& CenterLocation);
	/** Reverts to the previous camera state. */
	void RevertToPreviousCameraState(const FCameraStateTransition& Transition);

	float GetDistanceToTarget() const;
	
	/* Public Getters */
	FORCEINLINE FGameplayTag GetCurrentState() const { return CurrentStateData->StateTag; }
	FORCEINLINE bool IsLockCameraPosition() const { return bLockCameraPosition; }
	FORCEINLINE bool IsLockCameraRotation() const { return bLockCameraRotation; }

	/* Public Setters */
	FORCEINLINE void SetLockCameraPosition(const bool bLock) { bLockCameraPosition = bLock; }
	FORCEINLINE void SetLockCameraRotation(const bool bLock) { bLockCameraRotation = bLock; }

	
	FCameraStateChangeSignature OnCameraStateChanged;

protected:
	virtual void BeginPlay() override;

	// UPROPERTY(EditDefaultsOnly, Category = "CameraStateSystem")
	// TObjectPtr<UCameraStateData_Base> DefaultCameraStateData;

private:
	/** Instantly transitions to a new camera state. */
	void TransitionCamera_Instant(const UCameraStateData_Base* NewStateData, const FVector& CenterLocation);
	
	UPROPERTY()
	TObjectPtr<const UCameraStateData_Base> CurrentStateData;
	FVector CurrentCenterLocation;

	/** Previous camera state data. */
	UPROPERTY()
	TObjectPtr<const UCameraStateData_Base> PreviousStateData;
	FVector PreviousCenterLocation;

	bool bLockCameraPosition = false;
	bool bLockCameraRotation = false;

	
	/* State Transition */
	UFUNCTION()
	void OnTransitionUpdate(float Value); /** Updates the transition timeline. */
	UFUNCTION()
	void OnTransitionFinished(); /** Called when the transition timeline finishes. */
	
	UPROPERTY()
	TObjectPtr<UTimelineComponent> TransitionTimeline;
	FOnTimelineFloat TransitionUpdateDelegate;

	ECameraTransitionType CurrentTransitionType = ECameraTransitionType::Ctt_None;

	TTuple<TObjectPtr<const UCameraStateData_Base>, FVector> TransitionTargetParams;

	// Interpolation
	FCameraStatePOVData InterpStartData;
	bool bIsInterpolatingCam = false;
	float TransitionAlpha = 0.f;
	
	// Fade
	FLinearColor CurrentFadeColor;
	bool bFadeOut = false;
	
private:
	TMap<FGameplayTag, TUniquePtr<ICameraStateInterface>> CameraStateMap;
};
