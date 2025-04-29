// Rookies Group Project, CGS Team 9, 2025

#pragma once

#include "CoreMinimal.h"
#include "CameraStateSystem/StateCamGameplayTags.h"
#include "CameraStateSystem/CameraState/Data/CameraStateData_Base.h"
#include "CameraStateData_Static.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "CamStateData - Static")
class CAMERASTATESYSTEM_API UCameraStateData_Static : public UCameraStateData_Base
{
	GENERATED_BODY()
public:
	explicit UCameraStateData_Static (const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
	{
		StateTag = StateCamGameplayTags::CameraState_Static;
		GameplayAxis = EStateAxis::X_Right;
		RotationOffset = FRotator::ZeroRotator;
	}

	UPROPERTY(EditDefaultsOnly, Category="Core Static Settings",
		meta=(Tooltip="Distance from the Center Location to the camera"))
	float Distance = 800.f;

	UPROPERTY(EditDefaultsOnly, Category="Core Static Settings",
		meta=(Tooltip="Vertical offset from Center Location"))
	float HeightOffset = 300.f;

	UPROPERTY(EditDefaultsOnly, Category="Core Static Settings",
		meta=(Tooltip="Rotational offset from default camera orientation (when this is 0, the camera will face the axis straigt on)"))
	FRotator RotationOffset;

	
	// LookAt Settings
	UPROPERTY(EditDefaultsOnly, Category="LookAt Settings",
		meta=(Tooltip="Whether the camera should look at the target"))
	bool bLookAtTarget = false;

	UPROPERTY(EditDefaultsOnly, Category="LookAt Settings", meta=(EditCondition="bLookAtTarget",
		Tooltip="How much the camera should look at the target"))
	float LookAtInterpSpeed = 2.f;

	UPROPERTY(EditDefaultsOnly, Category="LookAt Settings", meta=(EditCondition="bLookAtTarget",
	  Tooltip="How strongly the look-at affects camera yaw rotation (0-1 range) 1 being Directily looking at the target", ClampMin="0", ClampMax="1"))
	float LookAtInfluenceYaw = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category="LookAt Settings", meta=(EditCondition="bLookAtTarget",
  Tooltip="How strongly the look-at affects camera pitch rotation (0-1 range) 1 being Directily looking at the target", ClampMin="0", ClampMax="1"))
	float LookAtInfluencePitch = 0.6f;
	
	UPROPERTY(EditDefaultsOnly, Category="LookAt Settings", meta=(EditCondition="bLookAtTarget",
		Tooltip="The maximum angle the camera can rotate to look at the target", ClampMin="0", ClampMax="90"))
	float MaxLookAtAngle = 45.f;

	// Deadzone Settings
	// UPROPERTY(EditDefaultsOnly, Category="Deadzone Settings", meta=(EditCondition="bLookAtTarget",
	// 	Tooltip="Whether the camera should use a deadzone for the look-at"))
	// bool bUseDeadzone = false;
	// 
	// UPROPERTY(EditDefaultsOnly, Category="Deadzone Settings", meta=(EditCondition="bLookAtTarget && bUseDeadzone",
	// 	Tooltip="The deadzone radius for the look-at"))
	// float LookAtDeadzoneAngle = 15.f;
};
