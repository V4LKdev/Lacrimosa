// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.


#include "LacrPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Lacrimosa/Data/InputData.h"
#include "Lacrimosa/Interface/InputReceiverInterface.h"

ALacrPlayerController::ALacrPlayerController(const FObjectInitializer& ObjectInitializer)
{
	bAutoManageActiveCameraTarget = false;
}

void ALacrPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	checkf (PlayerInputData, TEXT("PlayerInputData is not set in AT9PlayerController!"));

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check (Subsystem);

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(PlayerInputData->InputMappingContext, 0);

	UEnhancedInputComponent* Input =
		CastChecked<UEnhancedInputComponent>(InputComponent);
	checkf (Input, TEXT("Input is not an UEnhancedInputComponent"));

	
	if (PlayerInputData->MoveAction)
	{
		Input->BindActionValueLambda(PlayerInputData->MoveAction, ETriggerEvent::Triggered,[this](const FInputActionValue& Value)
			{
				if (GetPawnAsInputReceiver())
				{
					GetPawnAsInputReceiver()->InputMove(Value);
				}
			});
	}

	if (PlayerInputData->InteractAction)
	{
		Input->BindActionValueLambda(PlayerInputData->InteractAction, ETriggerEvent::Triggered,[this](const FInputActionValue& Value)
			{
				if (GetPawnAsInputReceiver())
				{
					GetPawnAsInputReceiver()->InputInteract(Value);
				}
			});
	}
}

TScriptInterface<IInputReceiverInterface> ALacrPlayerController::GetPawnAsInputReceiver()
{
	if (!PawnAsInputReceiver)
	{
		if (APawn* PossessedPawn = GetPawn())
		{
			IInputReceiverInterface* InputReceiver = Cast<IInputReceiverInterface>(PossessedPawn);
			if (InputReceiver)
			{
				PawnAsInputReceiver.SetObject(PossessedPawn);
				PawnAsInputReceiver.SetInterface(InputReceiver);
			}
		}
	}
	return PawnAsInputReceiver;
}
