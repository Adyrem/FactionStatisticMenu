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
	* Initializes everything needed to build the viewport and display the statistics. Can also be used to reinitialize the object.
	* @param main_data - data asset containing most importantly statistics and a mesh
	* @param main_animation_sequence - animation applied to the mesh if it is a skeletal mesh
	*/
		void InitializeWidget(UEntityDataAsset* main_data, UAnimSequence* main_animation_sequence);
	/**
	* Initializes everyting needed to build the main and comparison viewport. Can also be used to reinitialize the object.
	* @param main_data - data asset containing most importantly statistics and a mesh
	* @param main_animation_sequence - animation applied to the mesh if it is a skeletal mesh
	* @param comp_data - same as main data asset but for comparison
	* @param comp_animation_sequence - same as main animation but for comparison
	*/
	UFUNCTION(BlueprintCallable)
		void InitializeWidget(UEntityDataAsset* main_data, UAnimSequence* main_animation_sequence, UEntityDataAsset* comp_data, UAnimSequence* comp_animation_sequence);


	//need to be set, preferably in editor as they can be the same for all units except maybe special cases
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UScrollboxStatEntry> scroll_entry_widget;

	//default to 0,0,0
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector main_spawnpoint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector comparison_spawnpoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool benable_color_coded_values = true;

	//required, preferably through editor as it has to be added to a material
	//ignored if OverwriteViewport was called, otherwise needed
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UTextureRenderTarget2D* main_render_target;
	//Ignored if comparison_viewport is not present, otherwise needed
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UTextureRenderTarget2D* comparison_render_target;

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
	void OverwriteViewport(const TTuple<UImage*, AStatisticsViewportCharacter*> overwrite_from);

	/**
	* Returns a structure to call OverwriteViewport from. Removes the need to know the implementation.
	* @return A tuple of the image representing the viewport and the character seen in the viewport
	*/
	const TTuple<UImage*, AStatisticsViewportCharacter*> GetOverwriteStructure() const;

private:
	/**
	* Gives control to the character seen in the viewport
	*/
	UFUNCTION()
		void OnViewportMouseDown();
	UFUNCTION()
		void OnComparisonViewportMouseDown();

	UPROPERTY(meta = (BindWidget))
		UScrollBox* m_StatsScrollBox;
	UPROPERTY(meta = (BindWidgetOptional))
		UButton* m_CompareButton;

	UPROPERTY(meta = (BindWidget))
		UImage* main_viewport;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* m_EntityName;

	UPROPERTY(meta = (BindWidgetOptional))
		UImage* comparison_viewport;
	UPROPERTY(meta = (BindWidgetOptional))
		UTextBlock* m_ComparisonEntityName;

	/**
	* Adds certain stats to  the scrollbox, if that stat is not yet present in the scrollbox
	* @param main - main data asset with stats of the unit/building
	* @param comparison - if present, it's stats will be added to the scrollbox alongside the main entity stats
	*/
	void AddEntriesToScrollBox(UScrollBox* scroll_box, const UEntityDataAsset* main, const UEntityDataAsset* comparison = nullptr);
	/**
	* Hides all entries in the stats scroll box and adds the now needed ones.
	*/
	void RebuildScrollboxEntries();
	/**
	* Creates a basic UScrollboxStatEntry and sets its name element. Returns the widget with the same name if it already exists after reinitializing it
	* @param name - needs to be unique, as widgets need different names
	* @return Basic initialized scroll box entry widget
	*/
	UScrollboxStatEntry* PrepareScrollBoxEntry(const FName& name);
	/**
	* Applies the text color to a UScrollboxStatEntry based on int
	* @param difference - any int, intended to be the difference between 2 values
	* @param applied_to - the comparison value of UScrollboxStatEntry will be changed
	*/
	void ApplyColorBasedOnDifference(const int32& difference, UScrollboxStatEntry* applied_to) const;
	/**
	* Adds entires to the scrollbox specific to units
	* @param unit_asset - the main data asset
	* @param comparison_asset - optional data asset for the comparison unit
	*/
	void AddUnitStats(UScrollBox* scroll_box, const UUnitDataAsset* unit_asset, const UUnitDataAsset* comparison_asset = nullptr);
	void AddBuildingStats(UScrollBox* scroll_box, const UBuildingDataAsset* building_asset, const UBuildingDataAsset* comparison_asset = nullptr);
	void AddHeroStats(UScrollBox* scroll_box, const UUnitDataAsset* unit_asset, const UUnitDataAsset* comparison_asset = nullptr);
	/**
	* Creates a AVieportCharacter with an attached render target, that can be rendered to for example a UImage through a material
	* @param mesh - The character requires a skeletal or static mesh.
	* @param render_target - The render Target is attached to the USceneCaptureComponent2D of the character
	* @param spawn_location - Optional spawn coordinates, defaults to 0,0,0
	* @param animation - Optional animation that loops
	* @return The complete character that then can be possesed
	*/
	AStatisticsViewportCharacter* BuildViewport(
		UEntityDataAsset* data_asset,
		UTextureRenderTarget2D* render_target,
		const FVector& spawn_location = FVector(0, 0, 0),
		UAnimSequence* animation = nullptr
	) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UEntityDataAsset* m_MainDataAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UEntityDataAsset* m_ComparisonDataAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UAnimSequence* m_MainAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UAnimSequence* m_ComparisonAnimation;

	AStatisticsViewportCharacter* m_ViewportCharacter;
	AStatisticsViewportCharacter* m_ComparisonViewportCharacter;
	TMap<FName, UScrollboxStatEntry*> m_ScrollEntries;

	bool m_bViewportOverwritten = false;
};
