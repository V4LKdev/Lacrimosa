// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#include "CameraState_Static.h"

#include "CameraStateSystem/CameraState/Data/StateData/CameraStateData_Static.h"
#include "Kismet/KismetMathLibrary.h"

bool FCameraState_Static::StateEnter(const UCameraStateData_Base* Data, const FVector& InCenterLocation)
{
	if (!TCameraStateTemplate::StateEnter(Data, InCenterLocation))
	{
		return false;
	}

	SetStaticCameraVariables();
	
	return true;
}

void FCameraState_Static::SetStaticCameraVariables() const
{
	// Calculate the camera target location based on the center location and the offsets
	StaticCameraLocation = CenterLocation;
	StaticCameraLocation.Z = StateData->HeightOffset;
	
	switch (StateData->GameplayAxis)
	{
	case EStateAxis::X_Right:
		StaticCameraLocation.Y += StateData->Distance;
		StaticCameraRotation = FRotator(0.f, -90.f, 0.f);
		break;
	case EStateAxis::X_Left:
		StaticCameraLocation.Y -= StateData->Distance;
		StaticCameraRotation = FRotator(0.f, 90.f, 0.f);
		break;
	case EStateAxis::Y_Right:
		StaticCameraLocation.X += StateData->Distance;
		StaticCameraRotation = FRotator(0.f, 180.f, 0.f);
		break;
	case EStateAxis::Y_Left:
		StaticCameraLocation.X -= StateData->Distance;
		StaticCameraRotation = FRotator(0.f, 0.f, 0.f);
	default:
		UE_LOG(LogCameraStateSystem, Warning, TEXT("Invalid Gameplay Axis for Static Camera State: %s"), *UEnum::GetDisplayValueAsText(StateData->GameplayAxis).ToString());
	}
}

FCameraStatePOVData FCameraState_Static::CalculateStateRawPOV()
{
	FCameraStatePOVData OutPOVData;
	FTViewTarget ViewTarget = CameraStateManager->ViewTarget;
	ViewTarget.POV = CameraStateManager->GetCameraCacheView();

	OutPOVData.Location = GetStaticCameraLocation();
	OutPOVData.Rotation = CalculateCameraRotation(1.f, ViewTarget, true);

	return OutPOVData;
}

FCameraStatePOVData FCameraState_Static::StateUpdate(const float DeltaTime, const FTViewTarget& VT)
{
	FCameraStatePOVData OutPOVData;
	/**
	* Optional Features to add:
 	* Dynamic Framing of Relevant Objects
 	* Arrowkey nudge rotation
 	* Automatic zoom for the framing
 	*/

	OutPOVData.Location = GetStaticCameraLocation();

	OutPOVData.Rotation = CalculateCameraRotation(DeltaTime, VT);

	return OutPOVData;
}

FRotator FCameraState_Static::CalculateCameraRotation(const float DeltaTime, const FTViewTarget& VT, const bool bGetRaw) const
{
	FRotator FinalRotation = GetStaticCameraRotation();
	
	if (StateData->bLookAtTarget)
	{
		const FVector TargetLocation = VT.Target->GetActorLocation();
		const FRotator CurrentRotation = VT.POV.Rotation;

		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
			GetStaticCameraLocation(),
			TargetLocation
		);


		float InterpSpeed = StateData->LookAtInterpSpeed; 
		FRotator RotationDelta = FRotator::ZeroRotator;

		RotationDelta = (LookAtRotation - GetStaticCameraRotation()).GetNormalized();
		RotationDelta.Yaw *= StateData->LookAtInfluenceYaw;
		RotationDelta.Pitch *= StateData->LookAtInfluencePitch;
		// Clamp the yaw difference to the max look-at angle
		RotationDelta.Yaw = FMath::Clamp(RotationDelta.Yaw, -StateData->MaxLookAtAngle, StateData->MaxLookAtAngle);
		FinalRotation += RotationDelta;

		FinalRotation += StateData->RotationOffset;

		const FRotator InterpRotation = UKismetMathLibrary::RInterpTo(
			CurrentRotation,
			FinalRotation,
			DeltaTime,
			InterpSpeed
			);

		return bGetRaw ? InterpRotation : FinalRotation;
	}
	else
	{
		return  FinalRotation + StateData->RotationOffset;
	}
}



