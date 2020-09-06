// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "StatisticsOverviewWidget.generated.h"

class UTextBlock;
class UEntityDataAsset;
class UUnitDataAsset;
class UBuildingDataAsset;
class UScrollboxStatEntry;
class AStatisticsViewportCharacter;
class UImage;
class UButton;

/**
 * Widget with 1-2 Viewports with characters in it. Contains a Scrollbox for Statistics entries.
 */
UCLASS()
class FACTIONSTATISTICMENU_API UStatisticsOverviewWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/**
	* Destroys the Viewport character and the viewport in turn doesn't work anymore. The viewport is hidden
	*/
	UFUNCTION(BlueprintCallable)
		void DestroyViewport();

	//need to be set, preferably in editor as they can be the same for all units except maybe special cases
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UScrollboxStatEntry> scroll_entry_widget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UTextureRenderTarget2D* main_render_target;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector main_spawnpoint;
	//Ignored if comparison_viewport is not present. Otherwise needed
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UTextureRenderTarget2D* comparison_render_target;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector comparison_spawnpoint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool benable_color_coded_values = true;

	//These should not be set in blueprint, as they should be passed through c++ either way, overwriting it
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UEntityDataAsset* main_data_asset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UEntityDataAsset* comparison_data_asset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UAnimSequence* main_animation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UAnimSequence* comparison_animation;

	UPROPERTY(meta = (BindWidget))
		UImage* main_viewport;
	UPROPERTY(meta = (BindWidgetOptional))
		UImage* comparison_viewport;

	/**
	* Creates either a skeletal mesh or static mesh from a data asset.
	* @param data_asset - any UEntityDataAsset that has a mesh
	* @return - either the mesh found in data_asset or a nullptr
	*/
	static UStreamableRenderAsset* RetrieveMeshfromDataAsset(const UEntityDataAsset* data_asset);

	/**
	* Overwrites the main viewport with one that's already created
	* @param image_to_set - The Image which shows the Viewport, taken from another widget, keeps rotation
	* @param character_to_set - The Character in the Viewport of the other Widget
	*/
	void OverwriteViewport(UImage* image_to_set, AStatisticsViewportCharacter* character_to_set);

	AStatisticsViewportCharacter* viewport_character;

private:
	/**
	* Gives control to the character seen in the viewport
	*/
	UFUNCTION()
		void OnViewportMouseDown(); 
	UFUNCTION()
		void OnComparisonViewportMouseDown();

	UPROPERTY(meta = (BindWidgetOptional))
		UButton* m_CompareButton;
	UPROPERTY(meta = (BindWidget))
		UScrollBox* m_StatsScrollBox;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* m_EntityName;
	UPROPERTY(meta = (BindWidgetOptional))
		UTextBlock* m_ComparisonEntityName;

	void AddEntriesToScrollBox();
	/**
	* Creates a basic UScrollboxStatEntry and sets its name element
	* @param name - needs to be unique, as widgets need different names
	*/
	UScrollboxStatEntry* PrepareScrollBoxEntry(const FName& name);
	/**
	* Applies the text color to a UScrollboxStatEntry based on int
	* @param difference - any int, intended to be the difference between 2 values
	* @param applied_to - the comparison value of UScrollboxStatEntry will be changed
	*/
	void ApplyColorBasedOnDifference(const int32& difference, UScrollboxStatEntry* applied_to);
	/**
	* Adds entires to the scrollbox specific to units
	* @param unit_asset - the main data asset
	* @param comparison_asset - optional data asset for the comparison unit
	*/
	void AddUnitStats(const UUnitDataAsset* unit_asset, const UUnitDataAsset* comparison_asset = nullptr);
	void AddBuildingStats(const UBuildingDataAsset* building_asset, const UBuildingDataAsset* comparison_asset = nullptr);
	void AddHeroStats(const UUnitDataAsset* unit_asset, const UUnitDataAsset* comparison_asset = nullptr);
	/**
	* Creates a AVieportCharacter with an attached render target, that can be rendered to for example a UImage through a material
	* @param mesh - The character requires a skeletal or static mesh.
	* @param render_target - The render Target is attached to the USceneCaptureComponent2D of the character
	* @param spawn_location - Optional spawn coordinates, defaults to 0,0,0
	* @param animation - Optional animation that loops
	* @return The complete character that then can be possesed
	*/
	AStatisticsViewportCharacter* BuildViewport(
		UStreamableRenderAsset* mesh, 
		UTextureRenderTarget2D* render_target, 
		const FVector& spawn_location = FVector(0, 0, 0), 
		UAnimSequence* animation = nullptr
	);

	AStatisticsViewportCharacter* m_ComparisonViewportCharacter;
	bool m_bWasViewportOverwritten = false;
};
