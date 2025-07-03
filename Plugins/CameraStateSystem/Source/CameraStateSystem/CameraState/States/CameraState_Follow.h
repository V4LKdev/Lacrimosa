// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once
#include "CameraStateSystem/CameraState/CameraStateTemplate.h"
#include "CameraStateSystem/CameraState/Data/StateData/CameraStateData_Follow.h"

class CAMERASTATESYSTEM_API FCameraState_Follow final : public TCameraStateTemplate<UCameraStateData_Follow>
{
public:
	/** TCameraStateTemplate Interface */
	virtual FCameraStatePOVData StateUpdate(const float DeltaTime, const FTViewTarget& VT) override;
	virtual FCameraStatePOVData CalculateStateRawPOV() override;
	virtual void StateExit() override;
	/** End TCameraStateTemplate Interface */

private:
	FVector CalculateCameraLocation(float DeltaTime, const FTViewTarget& VT, bool bGetRaw = false) const;
	FRotator CalculateCameraRotation(float DeltaTime, const FTViewTarget& VT, bool bGetRaw = false) const;

	float CalculateDeadzoneAngle(const FVector& TargetLocation, const FVector& CameraLocation) const;
	float CalculateTargetRotationPitch(const FVector& TargetLocation, const FVector& CameraLocation) const;

	FVector RawTargetLocation = FVector::ZeroVector;
};
