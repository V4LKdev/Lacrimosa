// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CameraStateSystem/CameraState/CameraStateTemplate.h"
#include "CameraStateSystem/CameraState/Data/StateData/CameraStateData_Static.h"


class CAMERASTATESYSTEM_API FCameraState_Static final : public TCameraStateTemplate<UCameraStateData_Static>
{
public:
	/** TCameraStateTemplate Interface */
	virtual bool StateEnter(const UCameraStateData_Base* Data, const FVector& InCenterLocation) override;

	virtual FCameraStatePOVData StateUpdate(const float DeltaTime, const FTViewTarget& VT) override;
	virtual FCameraStatePOVData CalculateStateRawPOV() override;
	/** End TCameraStateTemplate Interface */

private:
	FRotator CalculateCameraRotation(const float DeltaTime, const FTViewTarget& VT, bool bGetRaw = false) const;

	void SetStaticCameraVariables() const;
	
	const FVector& GetStaticCameraLocation() const { return StaticCameraLocation; }
	const FRotator& GetStaticCameraRotation() const { return StaticCameraRotation; }
	
	mutable FVector StaticCameraLocation = FVector::ZeroVector;
	mutable FRotator StaticCameraRotation = FRotator::ZeroRotator;
};
