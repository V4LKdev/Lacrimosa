// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.


#pragma once

class UCameraStateData_Base;
struct FCameraStatePOVData;
class ACameraStateManager;

/**
 * \brief Interface for camera state management.
 */
class CAMERASTATESYSTEM_API ICameraStateInterface
{
public:
	virtual ~ICameraStateInterface() = default;

	// Initialize the camera state with the camera manager
	virtual void StateInit(const ACameraStateManager* InCameraManager) = 0;

	// Enter the state with specific data and center location
	virtual bool StateEnter(const UCameraStateData_Base* Data, const FVector& InCenterLocation) = 0;

	// Exit the state and reset data
	virtual void StateExit() = 0;

	// Update the state (pure virtual, must be implemented by derived classes)
	virtual FCameraStatePOVData StateUpdate(const float DeltaTime, const FTViewTarget& InVT) = 0;

	// Calculate the raw POV data for the state
	virtual FCameraStatePOVData StateRawPOV(const ACameraStateManager* InCameraManager, const UCameraStateData_Base* Data, const FVector& InCenterLocation) = 0;
};
