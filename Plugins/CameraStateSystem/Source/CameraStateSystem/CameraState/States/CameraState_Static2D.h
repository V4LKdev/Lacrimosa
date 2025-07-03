// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once
#include "CameraStateSystem/CameraState/CameraStateTemplate.h"
#include "CameraStateSystem/CameraState/Data/StateData/CameraStateData_Static2D.h"

class CAMERASTATESYSTEM_API FCameraState_Static2D final : public TCameraStateTemplate<UCameraStateData_Static2D>
{
public:
	/** TCameraStateTemplate Interface */
	virtual bool StateEnter(const UCameraStateData_Base* Data, const FVector& InCenterLocation) override;

	virtual FCameraStatePOVData StateUpdate(const float DeltaTime, const FTViewTarget& VT) override;
	virtual FCameraStatePOVData CalculateStateRawPOV() override;
	/** End TCameraStateTemplate Interface */
};
