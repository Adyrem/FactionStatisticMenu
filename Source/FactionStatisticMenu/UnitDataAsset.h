// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EntityDataAsset.h"
#include "UnitDataAsset.generated.h"

class UWeaponDataAsset;

/**
 * Temporary DataAsset class for Units. Should be deleted after being replaced by the real one
 */
UCLASS()
class FACTIONSTATISTICMENU_API UUnitDataAsset : public UEntityDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Construction")
		uint8 group_size = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Unit")
		USkeletalMesh* skeletal_mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Unit")
		float max_walk_speed = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Attack")
		UWeaponDataAsset* weapon;
};
