// Copyright Giant Rocket Games


#include "Player/LevelInfo.h"

int32 ULevelInfo::GetLevelForExp(int32 Exp) const
{
	int32 CurrentLevel = 0;

	for (int32 i = 0; i < LevelInfo.Num(); i++)	
	{
		FAuraLevelInfo Info = LevelInfo[i];
		if (Info.ExpRequirement >= Exp)
		{
			break;
		}
		CurrentLevel = i;
	}

	return CurrentLevel;
}

int32 ULevelInfo::GetExpRequiredForLevel(int32 Level) const
{
	Level = FMath::Max(Level, 0);
	Level = FMath::Min(Level, LevelInfo.Num() - 1);

	return LevelInfo[Level].ExpRequirement;
}
