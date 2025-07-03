// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LacrPlayerController.generated.h"

class IInputReceiverInterface;
class UInputData;
/**
 * 
 */
UCLASS()
class LACRIMOSA_API ALacrPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	explicit ALacrPlayerController (const FObjectInitializer& ObjectInitializer);

protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputData> PlayerInputData;

private:
	TScriptInterface<IInputReceiverInterface> GetPawnAsInputReceiver();
	
	TScriptInterface<IInputReceiverInterface> PawnAsInputReceiver;
};
