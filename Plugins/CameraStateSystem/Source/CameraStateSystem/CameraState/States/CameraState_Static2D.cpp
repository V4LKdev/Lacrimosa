#include "CameraState_Static2D.h"

bool FCameraState_Static2D::StateEnter(const UCameraStateData_Base* Data, const FVector& InCenterLocation)
{
	if (!TCameraStateTemplate::StateEnter(Data, InCenterLocation))
	{
		return false;
	}

	return true;
}

FCameraStatePOVData FCameraState_Static2D::StateUpdate(const float DeltaTime, const FTViewTarget& VT)
{
	FCameraStatePOVData NewStateData;
	NewStateData.Location = CenterLocation + FVector(10.f, 0.f, StateData->HeightOffset);
	
	NewStateData.Rotation = FRotator(0.f, -90.0f, 0.f);
	
	return NewStateData;
}

FCameraStatePOVData FCameraState_Static2D::CalculateStateRawPOV()
{
	FCameraStatePOVData NewStateData;
	NewStateData.Location = CenterLocation + FVector(10.f, 0.f, StateData->HeightOffset);
	
	NewStateData.Rotation = FRotator(0.f, -90.0f, 0.f);
	
	return NewStateData;
}
