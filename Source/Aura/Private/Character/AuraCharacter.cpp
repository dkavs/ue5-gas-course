// Copyright Giant Rocket Games

#include "GameFramework/CharacterMovementComponent.h"
#include "Character/AuraCharacter.h"

AAuraCharacter::AAuraCharacter()
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.f, 400.f, 0.f);
	Movement->bConstrainToPlane = true;
	Movement->bSnapToPlaneAtStart = true;


	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}
