// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

/**
 *
 */
UCLASS()
class FACTIONSTATISTICMENU_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float pre_attack_delay = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float delay_between_shots = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float firing_duration = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float damage = 100;
};
