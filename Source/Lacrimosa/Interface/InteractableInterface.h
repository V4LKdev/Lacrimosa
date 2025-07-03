// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"


UINTERFACE(BlueprintType)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LACRIMOSA_API IInteractableInterface
{
	GENERATED_BODY()
public:
	// Blueprint event for interaction
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interactable")
	void Interact(class ALacrCharacterInteraction* CharacterRef);

	// Blueprint event for getting interactable state
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	bool GetIsInteractable() const;
	virtual bool GetIsInteractable_Implementation() const { return true; }

	// Blueprint event for setting interactable state
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void SetInteractable(bool bNewInteractable);
	virtual void SetInteractable_Implementation(bool bNewInteractable) {}

	// Blueprint event for getting interactable location
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	FVector GetInteractableLocation() const;
	virtual FVector GetInteractableLocation_Implementation() const { return FVector::ZeroVector; }

	// Blueprint event for updating interactable
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void UpdateInteractable(float AlphaDistance, bool bIsSelected);
	virtual void UpdateInteractable_Implementation(float AlphaDistance, bool bIsSelected) {}

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interactable")
	void OnDialogueFinished();
	
};
