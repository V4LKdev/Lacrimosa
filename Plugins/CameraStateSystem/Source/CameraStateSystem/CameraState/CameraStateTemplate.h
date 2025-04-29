// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once
#include "CameraStateInterface.h"
#include "CameraStateSystem/Module/CameraStateSystem.h"
#include "CameraStateSystem/Utility/StateCamStructs.h"
#include "CameraStateSystem/CameraManager/CameraStateManager.h"

class UCameraStateData_Base;

template <typename TData>
class CAMERASTATESYSTEM_API TCameraStateTemplate : public ICameraStateInterface
{
public:
	// Initialize the camera state with the camera manager
	FORCEINLINE virtual void StateInit(const ACameraStateManager* InCameraManager) final override
	{
		CameraStateManager = InCameraManager;
	}

	// Enter the state with specific data and center location
	virtual bool StateEnter(const UCameraStateData_Base* Data, const FVector& InCenterLocation) override
	{
		const TData* SpecificData = Cast<TData>(Data);
		if (SpecificData)
		{
			StateData = SpecificData;
			CenterLocation = InCenterLocation;
			return true;
		}
		return false;
	}

	// Exit the state and reset data
	virtual void StateExit() override
	{
		StateData = nullptr;
		CenterLocation = FVector::ZeroVector;
	}

	// Update the state (pure virtual, must be implemented by derived classes)
	virtual FCameraStatePOVData StateUpdate(const float DeltaTime, const FTViewTarget& InVT) override = 0;

	// Calculate the raw POV data for the state
	virtual FCameraStatePOVData StateRawPOV(const ACameraStateManager* InCameraManager, const UCameraStateData_Base* Data, const FVector& InCenterLocation) final override
	{
		if (StateData != nullptr)
		{
			return CalculateStateRawPOV();
		}
		
		const TData* SpecificData = Cast<TData>(Data);
		if (SpecificData)
		{
			StateData = SpecificData;
			CenterLocation = InCenterLocation;
			return CalculateStateRawPOV();
		}
		else
		{
			UE_LOG(LogCameraStateSystem, Warning, TEXT("Wrong UCameraStateData entered"));
			return FCameraStatePOVData();
		}
	}

protected:
	// Calculate the raw POV data (pure virtual, must be implemented by derived classes)
	virtual FCameraStatePOVData CalculateStateRawPOV() = 0;
	
	UPROPERTY()
	TObjectPtr<const ACameraStateManager> CameraStateManager;
	TObjectPtr<const TData> StateData;
	FVector CenterLocation = FVector::ZeroVector;
};
