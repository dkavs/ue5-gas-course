// Copyright Giant Rocket Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent)
	void AddToExperience(int32 Amount);

	UFUNCTION(BlueprintNativeEvent)
	void AddToPlayerLevel(int32 Amount);

	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoints(int32 Amount);

	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoints(int32 Amount);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetExperience() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointsReward(int32 Level) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointsReward(int32 Level) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForExperience(int32 Amount) const;

	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();
};
