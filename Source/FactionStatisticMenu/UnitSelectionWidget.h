// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitSelectionWidget.generated.h"

class UEntityDataAsset;
class UUnitSelectionButton;
class UVerticalBox;
class UTextBlock;

/**
* The map needs an animation sequence aswell, easiest way to do that is by using a struct
*/
USTRUCT(BlueprintType)
struct FAssetWithAnimation
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UEntityDataAsset* data_asset;
	UPROPERTY(EditAnywhere)
	UAnimSequence* animation_sequence;
};

/**
 * Widget class for displaying the units of a given faction
 * Displays all units in the data asset maps
 */
UCLASS()
class FACTIONSTATISTICMENU_API UUnitSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/**
	* The assets for buildings, units and heroes are seperate to be able to add them to different boxes.
	* Could also be handled through an enum in the struct if needed
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FAssetWithAnimation> building_assets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FAssetWithAnimation> unit_assets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FAssetWithAnimation> heroes_assets;

	/**
	* Every new button added will use this type and a UTextBlock element will be added as a child
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UUnitSelectionButton> selection_button_type;
	/**
	* Type of textblock that will then be filled into the button
	* Haven't found an easy way to move this to the button itself
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UTextBlock> button_text_type;

private:
	UPROPERTY(meta = (BindWidget))
		UVerticalBox* m_BuildingBox;
	UPROPERTY(meta = (BindWidget))
		UVerticalBox* m_UnitBox;
	UPROPERTY(meta = (BindWidget))
		UVerticalBox* m_HeroesBox;

	/**
	* Creates new buttons of type selection_button_type and displays them
	* @param assets - buttons are constructed from this
	* @param vertical_box - element to add the new buttons to
	*/
	void AddAssetMapToBox(TMap<FString, FAssetWithAnimation> assets, UVerticalBox* vertical_box);
};
