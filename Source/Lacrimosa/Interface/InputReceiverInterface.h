// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputActionValue.h"
#include "InputReceiverInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInputReceiverInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LACRIMOSA_API IInputReceiverInterface
{
	GENERATED_BODY()

public:
	virtual void InputMove(const FInputActionValue& Value) = 0;
	virtual void InputInteract(const FInputActionValue& Value) = 0;
};
