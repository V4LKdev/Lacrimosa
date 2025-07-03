// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "LacrTwoWayTrigger.generated.h"

class UArrowComponent;

UENUM()
enum ETriggerSide
{
	None = 0,
	Positive = 1,
	Negative = 2
};

UCLASS()
class LACRIMOSA_API ALacrTwoWayTrigger : public ATriggerBox
{
	GENERATED_BODY()

public:
	ALacrTwoWayTrigger();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPositiveExit();
	UFUNCTION(BlueprintImplementableEvent)
	void OnNegativeExit();

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowPositive;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowNegative;

private:
	UFUNCTION()
	void Internal_TriggerSide(ETriggerSide TriggerSide);

	FVector GetLocationInWorldSpace(const FVector& LocalLocation) const
	{
		return GetActorTransform().TransformPosition(LocalLocation);
	}
	
	UPROPERTY()
	TMap<AActor*, TEnumAsByte<ETriggerSide>> ActorEntryQueue;

};
