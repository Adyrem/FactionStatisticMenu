// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EntityDataAsset.generated.h"

/**
 * DataAsset containing generic stats that apply to all types of Units/Buildings/Heroes
 */
UCLASS()
class FACTIONSTATISTICMENU_API UEntityDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Health")
		float maximum_health = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Construction")
		float resources_cost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Construction")
		float command_point_cost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "UI")
		FString ui_name;
};
