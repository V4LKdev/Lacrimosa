// Rookies Group Project, CGS Team 9, 2025

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "CameraStateData_Base.generated.h"

UENUM(BlueprintType)
enum class EStateAxis : uint8
{
	X_Right =	0 UMETA(DisplayName = "X Axis Right"),
	X_Left =	1 UMETA(DisplayName = "X Axis Left"),
	Y_Right =	2 UMETA(DisplayName = "Y Axis Right"),
	Y_Left =	3 UMETA(DisplayName = "Y Axis Left"),
};

/**
 * 
 */
UCLASS(Abstract)
class CAMERASTATESYSTEM_API UCameraStateData_Base : public UDataAsset
{
	GENERATED_BODY()
	
public:
	FGameplayTag StateTag;

	UPROPERTY(EditDefaultsOnly, Category="Core Settings",
	meta=(Tooltip="Primary axis for camera Rotation behavior"))
	EStateAxis GameplayAxis;
};
