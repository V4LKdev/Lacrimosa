// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Lacrimosa/Character/LacrCharacterBase.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTriggerSubsystem.generated.h"

struct FCameraStateTransition;
class ALacrCharacterBase;
class ALacrCharacterInteraction;
class ACameraStateManager;
class UDialogueData;
struct FCameraStateChange;

/**
 * TODO: Store and get previous camera state + location
 */
UCLASS(DisplayName = "Gameplay Trigger")
class LACRIMOSA_API UGameplayTriggerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	/** Camera */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Trigger | Camera", BlueprintPure = false)
	void ChangeCameraState(const FCameraStateChange& NewState, const FVector& CenterLocation) const;
	UFUNCTION(BlueprintCallable, Category = "Gameplay Trigger | Camera", BlueprintPure = false)
	void SetLockCameraPosition(const bool bLock) const;
	UFUNCTION(BlueprintCallable, Category = "Gameplay Trigger | Camera", BlueprintPure = false)
	void SetLockCameraRotation(const bool bLock) const;
	UFUNCTION(BlueprintCallable, Category = "Gameplay Trigger | Camera", BlueprintPure = false)
	void RevertToPreviousCameraState(FCameraStateTransition Transition) const;
	

	/** Character Movement */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Trigger | Character Movement", BlueprintPure = false)
	void LockMovementInput (const bool bLockMovement) const;
	UFUNCTION(BlueprintCallable, Category = "Gameplay Trigger | Character Movement", BlueprintPure = false)
	void ForceMoveTo(const FVector& TargetLocation) const;
	UFUNCTION(BlueprintCallable, Category = "Gameplay Trigger | Character Movement", BlueprintPure = false)
	void ForceMoveAlong(const EMoveAlongDirection Direction, const float Duration = 0.7f) const;

	/** Dialogue */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Trigger | Dialogue", BlueprintPure = false)
	void SetNewDialogue(AActor* InDialogueInstigator, UDialogueData* DialogueData) const;

private:
	ACameraStateManager* GetCameraStateManager() const;
	UPROPERTY()
	mutable TObjectPtr<ACameraStateManager> CameraStateManager;

	ALacrCharacterBase* GetBaseCharacter() const;
	UPROPERTY()
	mutable TObjectPtr<ALacrCharacterBase> BaseCharacter;

	ALacrCharacterInteraction* GetInteractionCharacter() const;
	UPROPERTY()
	mutable TObjectPtr<ALacrCharacterInteraction> InteractionCharacter;
};
