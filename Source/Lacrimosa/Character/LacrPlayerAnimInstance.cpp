// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.


#include "LacrPlayerAnimInstance.h"

#include "LacrCharacterBase.h"

FVector2D ULacrPlayerAnimInstance::GetCharacterDirection() const
{
	if (OwningCharacter)
	{
		FVector2D Direction = OwningCharacter->AbsoluteCharacterDirection;

		// Bias the X axis if the absolute values of X and Y are nearly equal
		if (FMath::IsNearlyEqual(FMath::Abs(Direction.X), FMath::Abs(Direction.Y)))
		{
			Direction.X += (Direction.X >= 0 ? KINDA_SMALL_NUMBER : -KINDA_SMALL_NUMBER);
		}

		return Direction;
	}

	return FVector2D::ZeroVector;
}

void ULacrPlayerAnimInstance::OnInit_Implementation()
{
	Super::OnInit_Implementation();
	
	if (GetOwningActor())
	{
		OwningCharacter = Cast<ALacrCharacterBase>(GetOwningActor());
		ensure(OwningCharacter);
	}
}