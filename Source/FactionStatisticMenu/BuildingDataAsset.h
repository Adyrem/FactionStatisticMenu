// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EntityDataAsset.h"
#include "Engine/StreamableRenderAsset.h"
#include "BuildingDataAsset.generated.h"

/**
 * Temporary DataAsset class for buildings. Should be deleted after being replaced by the real one
 */
UCLASS()
class FACTIONSTATISTICMENU_API UBuildingDataAsset : public UEntityDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bstart_construction_after_spawn = true;
	/**
	* UStreamableRenderAsset is the first common parent class of static and skeletal mesh. Textures can be assigned aswell but will not work
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Unit")
		UStreamableRenderAsset* static_or_skeletal_mesh;
};
