// Rookies Group Project, CGS Team 9, 2025

#pragma once

#include "CoreMinimal.h"
#include "CameraStateSystem/StateCamGameplayTags.h"
#include "CameraStateSystem/CameraState/Data/CameraStateData_Base.h"
#include "CameraStateData_Static2D.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "CamStateData - Static 2D")
class CAMERASTATESYSTEM_API UCameraStateData_Static2D : public UCameraStateData_Base
{
	GENERATED_BODY()
public:
	explicit UCameraStateData_Static2D (const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
	{
		StateTag = StateCamGameplayTags::CameraState_2D_Static;
		GameplayAxis = EStateAxis::X_Right;
	}

	UPROPERTY(EditDefaultsOnly, Category="Core Static Settings",
		meta=(Tooltip="Vertical offset from Center Location"))
	float HeightOffset = 300.f;
};
