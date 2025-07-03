// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.


#include "GameplayTriggerSubsystem.h"

#include "CameraStateSystem/CameraManager/CameraStateManager.h"
#include "Lacrimosa/Character/LacrCharacterInteraction.h"

void UGameplayTriggerSubsystem::ChangeCameraState(const FCameraStateChange& NewState, const FVector& CenterLocation) const
{
	if (GetCameraStateManager())
	{
		GetCameraStateManager()->ChangeCameraState(NewState, CenterLocation);
	}
}

void UGameplayTriggerSubsystem::SetLockCameraPosition(const bool bLock) const
{
	if (GetCameraStateManager())
	{
		GetCameraStateManager()->SetLockCameraPosition(bLock);
	}
}

void UGameplayTriggerSubsystem::SetLockCameraRotation(const bool bLock) const
{
	if (GetCameraStateManager())
	{
		GetCameraStateManager()->SetLockCameraRotation(bLock);
	}
}

void UGameplayTriggerSubsystem::RevertToPreviousCameraState(const FCameraStateTransition Transition) const
{
	if (GetCameraStateManager())
	{
		GetCameraStateManager()->RevertToPreviousCameraState(Transition);
	}
}


void UGameplayTriggerSubsystem::LockMovementInput(const bool bLockMovement) const
{
	if (GetBaseCharacter())
	{
		GetBaseCharacter()->LockMovementInput(bLockMovement);
	}
}

void UGameplayTriggerSubsystem::ForceMoveTo(const FVector& TargetLocation) const
{
	if (GetBaseCharacter())
	{
		GetBaseCharacter()->ForceMoveTo(TargetLocation);
	}
}

void UGameplayTriggerSubsystem::ForceMoveAlong(const EMoveAlongDirection Direction, const float Duration) const
{
	if (GetBaseCharacter())
	{
		GetBaseCharacter()->ForceMoveAlong(Direction, Duration);
	}
}

void UGameplayTriggerSubsystem::SetNewDialogue(AActor* InDialogueInstigator, UDialogueData* DialogueData) const
{
	if (GetInteractionCharacter())
	{
		GetInteractionCharacter()->SetNewDialogue(InDialogueInstigator, DialogueData);
	}
}



ACameraStateManager* UGameplayTriggerSubsystem::GetCameraStateManager() const
{
	if (!CameraStateManager
		&& GetWorld()
		&& GetWorld()->GetFirstPlayerController()
		&& Cast<ACameraStateManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager))
	{
		CameraStateManager = Cast<ACameraStateManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);
	}
	return CameraStateManager;
}

ALacrCharacterBase* UGameplayTriggerSubsystem::GetBaseCharacter() const
{
	if (!BaseCharacter
	&& GetWorld()
	&& GetWorld()->GetFirstPlayerController()
	&& Cast<ALacrCharacterBase>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		BaseCharacter = Cast<ALacrCharacterBase>(GetWorld()->GetFirstPlayerController()->GetPawn());
	}
	return BaseCharacter;
}

ALacrCharacterInteraction* UGameplayTriggerSubsystem::GetInteractionCharacter() const
{
	if (!InteractionCharacter
	&& GetWorld()
	&& GetWorld()->GetFirstPlayerController()
	&& Cast<ALacrCharacterInteraction>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		InteractionCharacter = Cast<ALacrCharacterInteraction>(GetWorld()->GetFirstPlayerController()->GetPawn());
	}
	return InteractionCharacter;
}
