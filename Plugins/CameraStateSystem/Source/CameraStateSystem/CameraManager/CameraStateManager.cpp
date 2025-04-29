// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.


#include "CameraStateManager.h"

#include "CameraStateSystem/StateCamGameplayTags.h"
#include "CameraStateSystem/CameraState/CameraStateInterface.h"
#include "CameraStateSystem/CameraState/States/CameraState_Follow.h"
#include "CameraStateSystem/CameraState/States/CameraState_Static.h"
#include "CameraStateSystem/CameraState/States/CameraState_Static2D.h"
#include "CameraStateSystem/Module/CameraStateSystem.h"


ACameraStateManager::ACameraStateManager(const FObjectInitializer&)
{
	/** Initialize the State Map **/
	CameraStateMap.Empty();
	CameraStateMap.Add(StateCamGameplayTags::CameraState_Follow, MakeUnique<FCameraState_Follow>());
	CameraStateMap.Add(StateCamGameplayTags::CameraState_Static, MakeUnique<FCameraState_Static>());
	CameraStateMap.Add(StateCamGameplayTags::CameraState_2D_Static, MakeUnique<FCameraState_Static2D>());
	
	TransitionTimeline = CreateDefaultSubobject<UTimelineComponent>(FName("TransitionTimeline"));
	TransitionTimeline->SetLooping(false);
}

void ACameraStateManager::BeginPlay()
{
	Super::BeginPlay();

	/** Bind Timeline Delegates **/
	TransitionUpdateDelegate.BindUFunction(this, FName("OnTransitionUpdate"));

	FOnTimelineEvent TransitionFinishedDelegate;
	TransitionFinishedDelegate.BindUFunction(this, FName("OnTransitionFinished"));
	TransitionTimeline->SetTimelineFinishedFunc(TransitionFinishedDelegate);

	/** Manually Set PlayerController View Target **/
	FViewTargetTransitionParams TransitionParams;
	TransitionParams.BlendTime = 0.f;
	TransitionParams.BlendFunction = VTBlend_PreBlended;
	TransitionParams.BlendExp = 0.f;
	TransitionParams.bLockOutgoing = false;
	PCOwner->SetViewTarget(PCOwner->GetPawn(), TransitionParams);

	/** Initialize all states **/
	for (const TTuple<FGameplayTag, TUniquePtr<ICameraStateInterface>>& StatePair : CameraStateMap)
	{
		StatePair.Value->StateInit(this);
	}

	/** Transition into the Default State **/
	// FCameraStateChange DefaultStateChange;
	// DefaultStateChange.CameraStateData = DefaultCameraStateData;
	// DefaultStateChange.Transition = FCameraStateTransition();
	// ChangeCameraState(DefaultStateChange, FVector::ZeroVector);
}

float ACameraStateManager::GetDistanceToTarget() const
{
	return FVector::Distance(GetCameraCacheView().Location, ViewTarget.Target->GetActorLocation());
}


void ACameraStateManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	/* Override the POV with the states custom calculated data from last frame */
	OutVT.POV.Location = GetCameraCacheView().Location;
	OutVT.POV.Rotation = GetCameraCacheView().Rotation;

	// Skip the state calculation altogether if both location and rotation are locked. this should be removed or extended later if states modify more than just these to values
	if (IsLockCameraPosition() && IsLockCameraRotation())
	{
		FillCameraCache(OutVT.POV);
		UE_LOG(LogCameraStateSystem, Verbose, TEXT("Skipping ViewTarget update due to locks"));
		return;
	}

	FCameraStatePOVData NewViewTargetPOV;

	// If we are in a valid state, calculate the states ViewTargetPOV
	if (CurrentStateData && CameraStateMap.Contains(GetCurrentState()))
	{
		NewViewTargetPOV = CameraStateMap[GetCurrentState()]->StateUpdate(DeltaTime, OutVT);
	}

	if (bIsInterpolatingCam)
	{
		// While interpolating the camera as transition, we don't use the direct output from the state but lerp towards it
		NewViewTargetPOV.Location = FMath::Lerp(InterpStartData.Location, NewViewTargetPOV.Location, TransitionAlpha);
		NewViewTargetPOV.Rotation = FMath::Lerp(InterpStartData.Rotation, NewViewTargetPOV.Rotation, TransitionAlpha);
	}

	// Overwrite the Location or Rotation with previous frame data in case its locked
	NewViewTargetPOV.Location = bLockCameraPosition ? OutVT.POV.Location : NewViewTargetPOV.Location;
	NewViewTargetPOV.Rotation = bLockCameraRotation ? OutVT.POV.Rotation : NewViewTargetPOV.Rotation;


	// Finally Set the OutViewTarget with the calculated data
	OutVT.POV.Location = NewViewTargetPOV.Location;
	OutVT.POV.Rotation = NewViewTargetPOV.Rotation;
	FillCameraCache(OutVT.POV);
}


void ACameraStateManager::ChangeCameraState(const FCameraStateChange& NewState, const FVector& CenterLocation)
{
	if (!NewState.IsValid() || NewState.CameraStateData == CurrentStateData || !CameraStateMap.Contains(
		NewState.CameraStateData->StateTag))
	{
		UE_LOG(LogCameraStateSystem, Warning, TEXT("Invalid camera state data or state already active."));
		return;
	}

	// Stop the transition timeline if we are already in a transition
	if (CurrentTransitionType != ECameraTransitionType::Ctt_None)
	{
		TransitionTimeline->Stop();
		bFadeOut = false;
		OnTransitionFinished();
		TransitionTargetParams = {NewState.CameraStateData, CenterLocation};
		TransitionAlpha = 0.f;
	}

	CurrentTransitionType = NewState.Transition.TransitionType;
	float TransitionDuration = NewState.Transition.TransitionDuration;

	switch (NewState.Transition.TransitionType)
	{
	case ECameraTransitionType::Ctt_Instant:
		TransitionCamera_Instant(NewState.CameraStateData, CenterLocation);
		return;
	case ECameraTransitionType::Ctt_Interp:
		{
			if (NewState.CameraStateData->StateTag.MatchesTag(StateCamGameplayTags::CameraState_2D))
			{
				UE_LOG(LogCameraStateSystem, Warning, TEXT("2D Camera State does not support Interpolation!"));
				TransitionCamera_Instant(NewState.CameraStateData, CenterLocation);
				return;
			}
			/**
			 *	Interpolation transitions states directly, but then overwrites the new states output in order to lerp from the InterpStartData
			 */
			InterpStartData.Location = ViewTarget.POV.Location;
			InterpStartData.Rotation = ViewTarget.POV.Rotation;

			if (CurrentStateData)
			{
				CameraStateMap[GetCurrentState()]->StateExit();
			}

			CameraStateMap[NewState.CameraStateData->StateTag]->StateEnter(NewState.CameraStateData, CenterLocation);
			bIsInterpolatingCam = true;
		
			PreviousStateData = CurrentStateData;
			PreviousCenterLocation = CurrentCenterLocation;
			
			CurrentStateData = NewState.CameraStateData;
			CurrentCenterLocation = CenterLocation;
			OnCameraStateChanged.Broadcast(CurrentStateData);
			break;
		}
	case ECameraTransitionType::Ctt_Fade:
		{
			// Half the TransitionDuration since its played twice, to fade out and fade back in
			TransitionDuration = FMath::Max(0.1f, NewState.Transition.TransitionDuration * 0.5);
			bFadeOut = true;
			CurrentFadeColor = NewState.Transition.FadeColor;
			// Lock camera position during fade to combat funky behaviour. maybe expose this later to the designers
			SetLockCameraPosition(true);
			break;
		}
	default:
		return;
	}

	if (NewState.Transition.TransitionCurve)
	{
		TransitionTargetParams = {NewState.CameraStateData, CenterLocation};

		TransitionTimeline->AddInterpFloat(NewState.Transition.TransitionCurve, TransitionUpdateDelegate,FName("Alpha"));

		/* Calculate and set the duration. Expects CurveFloats to be in range 0-1 */
		TransitionTimeline->SetPlayRate(1.f / TransitionDuration);

		TransitionTimeline->PlayFromStart();
	}
}

void ACameraStateManager::RevertToPreviousCameraState(const FCameraStateTransition& Transition)
{
	FCameraStateChange PreviousStateChange;
	PreviousStateChange.CameraStateData = PreviousStateData;
	PreviousStateChange.Transition = Transition;
	ChangeCameraState(PreviousStateChange, PreviousCenterLocation);
}


void ACameraStateManager::TransitionCamera_Instant(const UCameraStateData_Base* NewStateData, const FVector& CenterLocation)
{
	if (CurrentStateData)
	{
		CameraStateMap[GetCurrentState()]->StateExit();
	}
	else
	{
		CurrentStateData = NewStateData;
	}

	if (!CameraStateMap[NewStateData->StateTag]->StateEnter(NewStateData, CenterLocation))
	{
		UE_LOG(LogCameraStateSystem, Error, TEXT("Failed to enter camera state: %s"),
		       *NewStateData->StateTag.ToString());

		if (CurrentStateData && !CameraStateMap[GetCurrentState()]->StateEnter(CurrentStateData, CenterLocation))
		{
			UE_LOG(LogCameraStateSystem, Error, TEXT("Failed to revert to previous camera state."));
		}

		if (CurrentTransitionType == ECameraTransitionType::Ctt_Instant)
		{
			CurrentTransitionType = ECameraTransitionType::Ctt_None;
		}
		return;
	}
	
	const FCameraStatePOVData TargetPOV = CameraStateMap[NewStateData->StateTag]->StateRawPOV(
		this, NewStateData, CenterLocation);

	ViewTarget.POV.Location = TargetPOV.Location;
	ViewTarget.POV.Rotation = TargetPOV.Rotation;
	// Fills the cache with the new location for the update function to work correctly
	FillCameraCache(ViewTarget.POV);

	// Finalise state change
	PreviousStateData = CurrentStateData;
	PreviousCenterLocation = CurrentCenterLocation;
	
	CurrentStateData = NewStateData;
	CurrentCenterLocation = CenterLocation;
	SetGameCameraCutThisFrame();
	OnCameraStateChanged.Broadcast(CurrentStateData);

	// Reset the transition type only for instant transitions. This would cancel out the fade effect if we reset it without the check
	if (CurrentTransitionType == ECameraTransitionType::Ctt_Instant)
	{
		CurrentTransitionType = ECameraTransitionType::Ctt_None;
	}
}


#pragma region Transition Timeline

void ACameraStateManager::OnTransitionUpdate(const float Value)
{
	TransitionAlpha = Value;

	if (CurrentTransitionType == ECameraTransitionType::Ctt_Fade)
	{
		// Inverts the alpha for fading back in again
		const float ModifiedAlpha = bFadeOut ? Value : 1.f - Value;
		SetManualCameraFade(ModifiedAlpha, CurrentFadeColor, false);
	}
}

void ACameraStateManager::OnTransitionFinished()
{
	if (CurrentTransitionType == ECameraTransitionType::Ctt_Interp)
	{
		bIsInterpolatingCam = false;
		TransitionAlpha = 0.f;
		CurrentTransitionType = ECameraTransitionType::Ctt_None;
		InterpStartData = FCameraStatePOVData();
		TransitionTargetParams = {nullptr, FVector::ZeroVector};
		return;
	}
	
	if (CurrentTransitionType == ECameraTransitionType::Ctt_Fade)
	{
		if (bFadeOut)
		{
			// make sure the entire screen is faded out, then Instantly Transition to the new view
			SetManualCameraFade(1.f, CurrentFadeColor, false);
			TransitionCamera_Instant(TransitionTargetParams.Key, TransitionTargetParams.Value);

			// Invert the fade alpha, then restart the timeline after a brief moment for cinematic effect and to prevent visual glitches
			bFadeOut = false;

			FTimerHandle UnusedHandle;
			GetWorld()->GetTimerManager().SetTimer(UnusedHandle, [this]
			{
				TransitionTimeline->PlayFromStart();
			}, 0.15f, false);
		}
		else
		{
			SetManualCameraFade(0.f, FLinearColor::Black, false);
			StopCameraFade();

			SetLockCameraPosition(false);

			CurrentTransitionType = ECameraTransitionType::Ctt_None;
		}
	}
}

#pragma endregion // Transition Timeline
