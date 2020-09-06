// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "UnitSelectionButton.generated.h"

class UEntityDataAsset;

/**
 * Button containing information for which unit/building should be shown or compared to
 */
UCLASS()
class FACTIONSTATISTICMENU_API UUnitSelectionButton : public UButton
{
	GENERATED_BODY()

public:
	UUnitSelectionButton();

	/**
	* This data asset will be attached to the next widget
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UEntityDataAsset* data_asset;
	/**
	* The animation will be attached to the mesh found in the data asset
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UAnimSequence* animation_sequence;

private:
	UFUNCTION()
		virtual void OnButtonClicked();

};
