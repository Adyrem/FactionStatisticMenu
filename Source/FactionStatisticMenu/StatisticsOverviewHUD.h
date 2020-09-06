// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/DataAsset.h"
#include "StatisticsOverviewHUD.generated.h"

class UEntityDataAsset;
class UStatisticsOverviewWidget;

/**
 * Handles the flow from one widget to the next and what data is passed
 */
UCLASS()
class FACTIONSTATISTICMENU_API AStatisticsOverviewHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	/**
	 * Show the next widget in line when a UFactionSelectionButton is pressed, the widget is taken from UFactionSelectionButton
	 * @param faction_button - the specific button that was pressed
	 */
	UFUNCTION()
		void OnFactionButtonClick(const UFactionSelectionButton* faction_button);
	/**
	* Shows a widget of type UStatisticsOverviewWidget
	* @param unit_selection_button - requires a DataAsset to be set and can have an optional Animation Sequence
	*/
	UFUNCTION()
		void OnUnitSelectionClick(UUnitSelectionButton* unit_selection_button);
	/**
	* Handles what should be done when the back button is clicked. Most of the time this hides the current widget. Ends Comparison Mode
	* @param caller - This widget will be hidden
	*/
	UFUNCTION()
		void OnBackButtonClick(UWidget* caller);
	/**
	* Starts the comparison mode and then sets the main viewport of the comparison widget
	*/
	UFUNCTION()
		void OnComparisonButtonClick();

private:
	UPROPERTY(EditAnywhere)
		const TSubclassOf<UUserWidget> m_FactionOverview;
	UPROPERTY(EditAnywhere)
		const TSubclassOf<UStatisticsOverviewWidget> m_StatisticsOverview;
	UPROPERTY(EditAnywhere)
		const TSubclassOf<UStatisticsOverviewWidget> m_ComparisonOverview;

	UUserWidget* m_TopWidget;
	UUserWidget* m_FactionOverviewWidget;
	UUserWidget* m_UnitSelectionWidget;
	UStatisticsOverviewWidget* m_StatisticsWidget;
	UStatisticsOverviewWidget* m_ComparisonWidget;

	UUnitSelectionButton* m_MainSelectedUnit;
	bool m_bSelectingComparison = false;

};
