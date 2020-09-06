// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FactionStatisticMenuGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class FACTIONSTATISTICMENU_API AFactionStatisticMenuGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AFactionStatisticMenuGameModeBase();
	virtual void StartPlay() override;
};
