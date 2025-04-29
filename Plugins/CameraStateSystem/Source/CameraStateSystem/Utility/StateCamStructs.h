// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CameraStateSystem/CameraState/Data/CameraStateData_Base.h"
#include "StateCamStructs.generated.h"

class UCurveFloat;

UENUM(BlueprintType)
enum class ECameraTransitionType : uint8
{
    Ctt_None UMETA(hidden),
    Ctt_Instant UMETA(DisplayName = "Instant Cut"),
    Ctt_Interp UMETA(DisplayName = "Interpolate"),
    Ctt_Fade UMETA(DisplayName = "Fade")
};

USTRUCT(BlueprintType)
struct CAMERASTATESYSTEM_API FCameraStateTransition
{
    GENERATED_BODY()
    
public:
    FCameraStateTransition()
        : TransitionType(ECameraTransitionType::Ctt_Instant)
        , TransitionCurve(nullptr)
        , TransitionDuration(1.f)
        , FadeColor(FLinearColor::Black)
    {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECameraTransitionType TransitionType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "TransitionType == ECameraTransitionType::CTT_Interp || TransitionType == ECameraTransitionType::CTT_Fade", EditConditionHides))
    TObjectPtr<UCurveFloat> TransitionCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "TransitionType == ECameraTransitionType::CTT_Interp || TransitionType == ECameraTransitionType::CTT_Fade", EditConditionHides, ClampMin = "0.1", Units = "s"))
    float TransitionDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "TransitionType == ECameraTransitionType::CTT_Fade", EditConditionHides))
    FLinearColor FadeColor;
};

USTRUCT(BlueprintType)
struct CAMERASTATESYSTEM_API FCameraStateChange
{
    GENERATED_BODY()

public:
    FCameraStateChange()
        : CameraStateData(nullptr)
        , Transition(FCameraStateTransition())
    {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<const UCameraStateData_Base> CameraStateData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
    FCameraStateTransition Transition;

    bool IsValid() const
    {
        return CameraStateData != nullptr;
    }
};


struct CAMERASTATESYSTEM_API FCameraStatePOVData
{
    FCameraStatePOVData()
        : Location(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
    {}

public:
    FVector Location;
    FRotator Rotation;
};
