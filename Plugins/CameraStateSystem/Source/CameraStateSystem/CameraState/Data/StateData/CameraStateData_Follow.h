// Rookies Group Project, CGS Team 9, 2025

#pragma once

#include "CoreMinimal.h"
#include "CameraStateSystem/StateCamGameplayTags.h"
#include "CameraStateSystem/CameraState/Data/CameraStateData_Base.h"
#include "CameraStateSystem/Utility/StateCamStructs.h"
#include "CameraStateData_Follow.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "CamStateData - Follow")
class CAMERASTATESYSTEM_API UCameraStateData_Follow : public UCameraStateData_Base
{
GENERATED_BODY()
public:
	explicit UCameraStateData_Follow (const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
	{
		StateTag = StateCamGameplayTags::CameraState_Follow;
		GameplayAxis = EStateAxis::X_Right;
	}
	
	UPROPERTY(EditDefaultsOnly, Category="Core Follow Settings",
		  meta=(Tooltip="Horizontal offset from follow position"))
	float LaneOffset = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Core Follow Settings",
			  meta=(Tooltip="Interpolation speed for camera position updates, 0 is Instant", ClampMin="0"))
	float FollowInterpSpeed = 5.f;
	
	// Distance Settings
	UPROPERTY(EditDefaultsOnly, Category="Distance Settings",
		  meta=(Tooltip="Distance from the Center Location / Target to the camera"))
	float Distance = 500.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Distance Settings",
		meta=(Tooltip="If the distance is relative to the target or the 'Absolute Center Location'"))
	bool bAbsoluteDistance = true;
	
	
	// Height Adjustments
	UPROPERTY(EditDefaultsOnly, Category="Height Settings",
		  meta=(Tooltip="Vertical offset from target position"))
	float BaseHeightOffset = 100.f;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category="Height Settings",
		  meta=(Tooltip="How much height changes with distance from target"))
	float HeightDistanceMultiplier = 0.1f;


	// Pitch Adjustments
	UPROPERTY(EditDefaultsOnly, Category="Camera Pitch",
		  meta=(Tooltip="Base camera pitch angle, acts as offset when looking at target", ClampMin="-90", ClampMax="90"))
	float PitchAngle = 0.f;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category="Camera Pitch",
		  meta=(Tooltip="How much camera pitch changes with distance"))
	float PitchDistanceMultiplier = 0.015f;
	
	
	// LookAt Behavior
	UPROPERTY(EditDefaultsOnly, Category="LookAt Settings",
			  meta=(Tooltip="Should camera automatically rotate toward target?"))
	bool bLookAtTarget = true;

	UPROPERTY(EditDefaultsOnly, Category="LookAt Settings", meta=(EditCondition="bLookAtTarget",
			  Tooltip="Base rotation speed when tracking target", ClampMin="0"))
	float LookAtInterpSpeed = 2.f;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category="LookAt Settings", meta=(EditCondition="bLookAtTarget",
		  Tooltip="Rotation speed while target is within deadzone, to prevent sudden rapid movement from deadzone rim to center it should be slower than the default LookAtInterpSpeed", ClampMin="0"))
	float DeadzoneLookAtInterpSpeed = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="LookAt Settings", meta=(EditCondition="bLookAtTarget",
		  Tooltip="How strongly the look-at affects camera rotation (0-1 range) 1 being Directily looking at the target", ClampMin="0", ClampMax="1"))
	float LookAtInfluence = .35f;

	UPROPERTY(EditDefaultsOnly, Category="LookAt Settings", meta=(EditCondition="bLookAtTarget",
		  Tooltip="Angle threshold where look-at tracking becomes active", ClampMin="0", ClampMax="90"))
	float LookAtDeadzoneAngle = 12.f;


	// Deadzone Settings
	// Possibly make it dependent on Angle distance to the target
	UPROPERTY(EditDefaultsOnly, Category="Deadzone Settings",
		  meta=(Tooltip="Distance threshold for position updates", ClampMin="0"))
	float LocationDeadzone = 200.f;
};
