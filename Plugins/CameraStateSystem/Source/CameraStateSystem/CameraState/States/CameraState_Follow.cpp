#include "CameraState_Follow.h"

#include "CameraStateSystem/CameraState/Data/StateData/CameraStateData_Follow.h"
#include "Kismet/KismetMathLibrary.h"

FCameraStatePOVData FCameraState_Follow::CalculateStateRawPOV()
{
	FCameraStatePOVData OutPOVData;
	FTViewTarget ViewTarget = CameraStateManager->ViewTarget;
	ViewTarget.POV = CameraStateManager->GetCameraCacheView();

	OutPOVData.Location = CalculateCameraLocation(1.f, ViewTarget, true);
	RawTargetLocation = OutPOVData.Location; // this is used to accurately calculate the camera rotation
	OutPOVData.Rotation = CalculateCameraRotation(1.f, ViewTarget, true);

	return OutPOVData;
}

void FCameraState_Follow::StateExit()
{
	TCameraStateTemplate<UCameraStateData_Follow>::StateExit();

	RawTargetLocation = FVector::ZeroVector;
}

FCameraStatePOVData FCameraState_Follow::StateUpdate(const float DeltaTime, const FTViewTarget& VT)
{
	FCameraStatePOVData OutPOVData;

	OutPOVData.Location = CalculateCameraLocation(DeltaTime, VT);
	OutPOVData.Rotation = CalculateCameraRotation(DeltaTime, VT);
	return OutPOVData;
}


FVector FCameraState_Follow::CalculateCameraLocation(const float DeltaTime, const FTViewTarget& VT, const bool bGetRaw) const
{
	const FVector CurrentLocation = VT.POV.Location;
	const FVector TargetLocation = VT.Target->GetActorLocation();
	FVector FinalLocation = TargetLocation;

	const float DeadzoneHalf = bGetRaw ? 0.0f : StateData->LocationDeadzone * 0.5f;

	switch (StateData->GameplayAxis)
	{
	case EStateAxis::X_Right:
	case EStateAxis::X_Left:
		{
			// Static Deadzone calculation
			const float DeltaX = FinalLocation.X - CurrentLocation.X;

			if (UKismetMathLibrary::Abs(DeltaX) > DeadzoneHalf)
			{
				FinalLocation.X = FinalLocation.X - UKismetMathLibrary::SignOfFloat(DeltaX) * DeadzoneHalf;
			}
			else
			{
				FinalLocation.X = CurrentLocation.X;
			}

			FinalLocation.X += StateData->LaneOffset;

			const float DistanceOrigin = StateData->bAbsoluteDistance ? CenterLocation.Y : TargetLocation.Y;

			const float DistanceOffset = StateData->GameplayAxis == EStateAxis::X_Right
				                             ? StateData->Distance
				                             : -StateData->Distance;
			FinalLocation.Y = DistanceOrigin + DistanceOffset;

			break;
		}
	case EStateAxis::Y_Right:
	case EStateAxis::Y_Left:
		{
			const float DeltaY = FinalLocation.Y - CurrentLocation.Y;

			if (UKismetMathLibrary::Abs(DeltaY) > DeadzoneHalf)
			{
				FinalLocation.Y = FinalLocation.Y - UKismetMathLibrary::SignOfFloat(DeltaY) * DeadzoneHalf;
			}
			else
			{
				FinalLocation.Y = CurrentLocation.Y;
			}

			FinalLocation.Y += StateData->LaneOffset;

			const float DistanceOrigin = StateData->bAbsoluteDistance ? CenterLocation.X : TargetLocation.X;
			const float DistanceOffset = StateData->GameplayAxis == EStateAxis::Y_Right
				                             ? StateData->Distance
				                             : -StateData->Distance;
			FinalLocation.X = DistanceOrigin + DistanceOffset;


			break;
		}
	default:
		UE_LOG(LogCameraStateSystem, Error, TEXT("Invalid Axis input in CameraState_FollowData"));
	}

	const float DistToTarget = UKismetMathLibrary::Max(
		UKismetMathLibrary::Vector_Distance(TargetLocation, bGetRaw ? FinalLocation : CurrentLocation),
		KINDA_SMALL_NUMBER);
	FinalLocation.Z += StateData->BaseHeightOffset + DistToTarget * StateData->HeightDistanceMultiplier;

	return UKismetMathLibrary::VInterpTo(
		CurrentLocation,
		FinalLocation,
		DeltaTime,
		bGetRaw ? 0.f : StateData->FollowInterpSpeed
	);
}

FRotator FCameraState_Follow::CalculateCameraRotation(const float DeltaTime, const FTViewTarget& VT,
                                                      const bool bGetRaw) const
{
	const FVector CameraLocation = bGetRaw ? RawTargetLocation : VT.POV.Location;
	const FVector TargetLocation = VT.Target->GetActorLocation();
	const FRotator CurrentRotation = VT.POV.Rotation;
	FRotator TargetRotation = FRotator();


	// Set Base Rotation dependent on follow axis
	if (StateData->GameplayAxis == EStateAxis::X_Right)
	{
		TargetRotation.Yaw = -90.f;
	}
	else if (StateData->GameplayAxis == EStateAxis::X_Left)
	{
		TargetRotation.Yaw = 90.f;
	}
	else if (StateData->GameplayAxis == EStateAxis::Y_Left)
	{
		TargetRotation.Yaw = 0.f;
	}
	else if (StateData->GameplayAxis == EStateAxis::Y_Right)
	{
		TargetRotation.Yaw = 180.f;
	}

	if (StateData->bLookAtTarget)
	{
		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
			CameraLocation,
			TargetLocation
		);

		const float AngleToTarget = CalculateDeadzoneAngle(TargetLocation, CameraLocation);
		const bool bInDeadzone = AngleToTarget < StateData->LookAtDeadzoneAngle;

		if (!bInDeadzone || bGetRaw)
		{
			// Apply look-at rotation with intensity scaling
			const FRotator RotationDelta = (LookAtRotation - TargetRotation).GetNormalized() * StateData->
				LookAtInfluence;
			TargetRotation += RotationDelta;
		}

		// Determine interpolation speed based on deadzone state
		const float InterpSpeed = bInDeadzone ? StateData->DeadzoneLookAtInterpSpeed : StateData->LookAtInterpSpeed;

		// Calculate the target rotation pitch with distance deviation
		TargetRotation.Pitch = CalculateTargetRotationPitch(TargetLocation, CameraLocation);

		// Interpolate to the target rotation
		const FRotator InterpRot = UKismetMathLibrary::RInterpTo(
			CurrentRotation,
			TargetRotation,
			DeltaTime,
			InterpSpeed
		);

		return bGetRaw ? TargetRotation : InterpRot;
	}
	else
	{
		TargetRotation.Pitch = CalculateTargetRotationPitch(TargetLocation, CameraLocation);
		return TargetRotation;
	}
}

float FCameraState_Follow::CalculateDeadzoneAngle(const FVector& TargetLocation, const FVector& CameraLocation) const
{
	FVector CameraViewForward;

	if (StateData->GameplayAxis == EStateAxis::X_Right)
	{
		CameraViewForward = FVector(0.f, -1.f, 0.f);
	}
	else if (StateData->GameplayAxis == EStateAxis::X_Left)
	{
		CameraViewForward = FVector(0.f, 1.f, 0.f);
	}
	else if (StateData->GameplayAxis == EStateAxis::Y_Right)
	{
		CameraViewForward = FVector(-1.f, 0.f, 0.f);
	}
	else if (StateData->GameplayAxis == EStateAxis::Y_Left)
	{
		CameraViewForward = FVector(1.f, 0.f, 0.f);
	}

	const FVector DirToTarget = (TargetLocation - CameraLocation).GetSafeNormal();
	const float AngleToTarget = UKismetMathLibrary::RadiansToDegrees(
		UKismetMathLibrary::Acos(
			FVector::DotProduct(CameraViewForward, DirToTarget))
	);
	return AngleToTarget;
}

float FCameraState_Follow::CalculateTargetRotationPitch(const FVector& TargetLocation,
                                                        const FVector& CameraLocation) const
{
	const float DistToTarget = UKismetMathLibrary::Vector_Distance(TargetLocation, CameraLocation);
	const float AdjustedPitch = StateData->PitchAngle - (DistToTarget * StateData->PitchDistanceMultiplier);
	return UKismetMathLibrary::FClamp(AdjustedPitch, -70.f, 70.f);
}
