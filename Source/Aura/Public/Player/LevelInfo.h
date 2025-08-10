// Copyright Giant Rocket Games

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelInfo.generated.h"


USTRUCT(BlueprintType)
struct FAuraLevelInfo
{

	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ExpRequirement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 AttributePointReward = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 SpellPointReward = 1;
};

/**
 * 
 */
UCLASS()
class AURA_API ULevelInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Info")
	TArray<FAuraLevelInfo> LevelInfo;

	int32 GetLevelForExp(int32 Exp) const;
	int32 GetExpRequiredForLevel(int32 Level) const;
};
