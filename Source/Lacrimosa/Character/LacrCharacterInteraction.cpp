// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.


#include "LacrCharacterInteraction.h"

#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Lacrimosa/Interface/InteractableInterface.h"
#include "Lacrimosa/Widget/DialogueWidget.h"
#include "Lacrimosa/Widget/GameHUD.h"

ALacrCharacterInteraction::ALacrCharacterInteraction()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetSphereRadius(InteractionDetectionRadius);
}

void ALacrCharacterInteraction::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	InteractionSphere->SetSphereRadius(InteractionDetectionRadius);
}

void ALacrCharacterInteraction::BeginPlay()
{
	Super::BeginPlay();

	if (InteractionSphere)
	{
		InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnInteractableBeginOverlap);
		InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnInteractableEndOverlap);
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	check(PlayerController);

	if (UUserWidget* Widget = CreateWidget(PlayerController, GameHUDClass))
	{
		GameHUD = Cast<UGameHUD>(Widget);
		if (!GameHUD)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to cast widget to UGameHUD"));
			return;
		}
		GameHUD->AddToViewport();

		GameHUD->DialogueWidget->OnWritingFinished.AddDynamic(this, &ThisClass::OnTextFinishedWriting);
	}
}



void ALacrCharacterInteraction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OverlappingInteractables.Num() > 0)
	{
		UpdateInteractables();	
	}
}

void ALacrCharacterInteraction::UpdateInteractables()
{
	// Initialize the closest interactable with a high default distance
    TTuple<AActor*, float> ClosestInteractable = MakeTuple(nullptr, 9999.f);

    // Iterate through all overlapping interactables
    for (AActor* Interactable : OverlappingInteractables)
    {
        // Check if the actor implements the interactable interface
        if (Interactable && Interactable->Implements<UInteractableInterface>())
        {
        	float CurrentElementDistance = 999.f;
        	float CurrentElementAlphaDistance = 0.f;
        	
            // Check if the interactable is enabled
            if (IInteractableInterface::Execute_GetIsInteractable(Interactable))
            {
                // Calculate the distance and alpha distance
                FVector InteractableLocation = IInteractableInterface::Execute_GetInteractableLocation(Interactable);
                CurrentElementDistance = FVector::Distance(GetActorLocation(), InteractableLocation);

                CurrentElementAlphaDistance = UKismetMathLibrary::MapRangeClamped(
                    CurrentElementDistance,
                    InteractionDetectionRadius,
                    InteractionRadius,
                    0.f,
                    1.f
                );

                // Update the closest interactable if within range
                if (CurrentElementAlphaDistance >= 1.f &&
                    (ClosestInteractable.Key == nullptr || CurrentElementDistance < ClosestInteractable.Value))
                {
                    ClosestInteractable = MakeTuple(Interactable, CurrentElementDistance);
                }
            }

            // Update the interactable's visual state
            IInteractableInterface::Execute_UpdateInteractable(Interactable, CurrentElementAlphaDistance, false);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Interactable %s does not implement IInteractableInterface"), *Interactable->GetName());
        }
    }

    // Set the closest interactable as the current interactable, Can also be null here
    CurrentInteractable = ClosestInteractable.Key;
    if (IsValid(CurrentInteractable))
    {
        // Highlight the closest interactable
        IInteractableInterface::Execute_UpdateInteractable(CurrentInteractable, 1.f, true);
    }
}


void ALacrCharacterInteraction::InputInteract(const FInputActionValue& Value)
{
	if (bHasActiveDialogue)
	{
		GetWorldTimerManager().ClearTimer(DialogueAutoProgressTimerHandle);
		if (bIsLastText)
		{
			FinishDialogue();
		}
		else
		{
			ProgressActiveDialogue();
		}
	}
	else if (IsValid(CurrentInteractable))
	{
		IInteractableInterface::Execute_Interact(CurrentInteractable, this);
	}
}

void ALacrCharacterInteraction::OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->Implements<UInteractableInterface>())
	{
		OverlappingInteractables.AddUnique(OtherActor);
	}
}

void ALacrCharacterInteraction::OnInteractableEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OverlappingInteractables.Contains(OtherActor))
	{
		OverlappingInteractables.Remove(OtherActor);

		if (DialogueInstigator && DialogueInstigator == OtherActor)
		{
			bDisableInteractableAfterDialogue = false;
			FinishDialogue();
		}
	}
}




void ALacrCharacterInteraction::SetNewDialogue(AActor* InDialogueInstigator, UDialogueData* DialogueData)
{
	DialogueInstigator = InDialogueInstigator;
	CurrentDialogueData = DialogueData;

	if (IsValid(CurrentDialogueData) && CurrentDialogueData->bLockPlayerMovement)
	{
		LockMovementInput(true);
	}

	bHasActiveDialogue = true;
	bIsLastText = false;
	bDisableInteractableAfterDialogue = true;
	DialogueIndex = 0;

	if (GameHUD)
	{
		GameHUD->DialogueWidget->FadeVisibility(true);
		ProgressActiveDialogue();
	}
}

void ALacrCharacterInteraction::OnTextFinishedWriting()
{
	if (IsValid(CurrentDialogueData))
	{
		const bool bShouldAutoProgress = bIsLastText || CurrentDialogueData->bAutoProgress;

		if (bShouldAutoProgress)
		{
			const FTimerDelegate TimerDelegate = bIsLastText ?
					FTimerDelegate::CreateUObject(this, &ThisClass::FinishDialogue) :
					FTimerDelegate::CreateUObject(this, &ThisClass::ProgressActiveDialogue);

			GetWorldTimerManager().SetTimer(
				DialogueAutoProgressTimerHandle,
				TimerDelegate,
				CurrentDialogueData->AutoProgressTime,
				false
			);
		}
	}
}

void ALacrCharacterInteraction::ProgressActiveDialogue()
{
	if (!IsValid(CurrentDialogueData) || !IsValid(GameHUD))
	{
		return;
	}

	GameHUD->DialogueWidget->DisplayText(CurrentDialogueData->DialogueLines[DialogueIndex]);
	DialogueIndex++;

	if (DialogueIndex >= CurrentDialogueData->DialogueLines.Num())
	{
		if (CurrentDialogueData->bLoopDialogue)
		{
			DialogueIndex = 0;
		}
		else
		{
			bIsLastText = true;
		}
	}
}

void ALacrCharacterInteraction::FinishDialogue()
{
	if (!IsValid(CurrentDialogueData) || !IsValid(GameHUD))
	{
		return;
	}

	GameHUD->DialogueWidget->FadeVisibility(false);
	bHasActiveDialogue = false;
	
	if (CurrentDialogueData->bLockPlayerMovement)
	{
		LockMovementInput(false);
	}

	if (IsValid(DialogueInstigator) && DialogueInstigator->Implements<UInteractableInterface>())
	{
		IInteractableInterface::Execute_OnDialogueFinished(DialogueInstigator);

		if (bDisableInteractableAfterDialogue)
		{
			IInteractableInterface::Execute_SetInteractable(DialogueInstigator, false);
		}
	}
	
	CurrentDialogueData = nullptr;
	DialogueInstigator = nullptr;
}
