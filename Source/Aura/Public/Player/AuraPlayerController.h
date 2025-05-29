// Copyright Giant Rocket Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"


class UInputMappingContext;
class UInputAction;
class IInteractableInterface;
class UAuraInputConfig;
class UAuraAbilitySystemComponent;
class USplineComponent;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	bool bShiftKeyDown;
	void Move(const FInputActionValue& InputActionValue);
	void ShiftStart() { bShiftKeyDown = true; }
	void ShiftEnd() { bShiftKeyDown = false; }
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	void CursorTrace();
	
	TScriptInterface<IInteractableInterface> LastActor;
	TScriptInterface<IInteractableInterface> ThisActor;
	FHitResult CursorHit;


	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraASC;

	UAuraAbilitySystemComponent* GetASC();
	void AutoRun();


	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;

	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadiance = 50.f;

	UPROPERTY()
	TObjectPtr<USplineComponent> Spline;
};
