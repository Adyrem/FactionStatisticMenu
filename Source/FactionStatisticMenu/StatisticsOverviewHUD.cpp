// Fill out your copyright notice in the Description page of Project Settings.


#include "StatisticsOverviewHUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "FactionSelectionButton.h"
#include "UnitSelectionButton.h"
#include "StatisticsOverviewWidget.h"

void AStatisticsOverviewHUD::BeginPlay()
{
	Super::BeginPlay();

	if (m_FactionOverview)
	{
		m_FactionOverviewWidget = CreateWidget(GetWorld(), m_FactionOverview);
		if (m_FactionOverviewWidget)
		{
			m_TopWidget = m_FactionOverviewWidget;
			m_FactionOverviewWidget->AddToViewport();
		}
	}

	APlayerController* player_controller = GetWorld()->GetFirstPlayerController();
	if (player_controller)
	{
		player_controller->bShowMouseCursor = true;
	}
}

void AStatisticsOverviewHUD::OnFactionButtonClick(const UFactionSelectionButton* faction_button)
{
	if (faction_button->called_widget)
	{
		m_UnitSelectionWidget = CreateWidget(GetWorld(), faction_button->called_widget);
		if (m_UnitSelectionWidget)
		{
			m_TopWidget = m_UnitSelectionWidget;
			m_UnitSelectionWidget->AddToViewport();
		}
	}
}

void AStatisticsOverviewHUD::OnUnitSelectionClick(UUnitSelectionButton* unit_selection_button)
{
	if (m_MainSelectedUnit && m_bSelectingComparison)
	{
		if (m_MainSelectedUnit == unit_selection_button)
		{
			//don't allow comparison of the unit on itself
			//Adding a version button to the statistics window will probably work bettter but otherwise special funtionality could be added here
			return;
		}

		if (unit_selection_button && m_ComparisonOverview)
		{
			m_ComparisonWidget = CreateWidget<UStatisticsOverviewWidget>(GetWorld(), m_ComparisonOverview);
			if (m_ComparisonWidget)
			{
				m_ComparisonWidget->main_data_asset = m_MainSelectedUnit->data_asset;
				m_ComparisonWidget->main_animation = m_MainSelectedUnit->animation_sequence;
				m_ComparisonWidget->OverwriteViewport(m_StatisticsWidget->main_viewport, m_StatisticsWidget->viewport_character);

				if (unit_selection_button->data_asset)
				{
					m_ComparisonWidget->comparison_data_asset = unit_selection_button->data_asset;
				}
				if (unit_selection_button->animation_sequence)
				{
					m_ComparisonWidget->comparison_animation = unit_selection_button->animation_sequence;
				}
				m_TopWidget = m_ComparisonWidget;
				m_ComparisonWidget->AddToViewport();
			}
		}
	}
	else if (unit_selection_button && m_StatisticsOverview)
	{
		//cant work with UUserWidget class because data assets need to be set
		m_StatisticsWidget = CreateWidget<UStatisticsOverviewWidget>(GetWorld(), m_StatisticsOverview);
		if (m_StatisticsWidget)
		{
			if (unit_selection_button->data_asset)
			{
				m_StatisticsWidget->main_data_asset = unit_selection_button->data_asset;
			}
			if (unit_selection_button->animation_sequence)
			{
				m_StatisticsWidget->main_animation = unit_selection_button->animation_sequence;
			}
			m_TopWidget = m_StatisticsWidget;
			m_StatisticsWidget->AddToViewport();
			m_MainSelectedUnit = unit_selection_button;
		}
	}
}

void AStatisticsOverviewHUD::OnBackButtonClick(UWidget* caller)
{
	if (caller)
	{
		caller->SetVisibility(ESlateVisibility::Collapsed);

		if (m_TopWidget == m_StatisticsWidget)
		{
			m_StatisticsWidget->DestroyViewport();
			m_TopWidget = m_UnitSelectionWidget;
		}
		else if (m_TopWidget == m_ComparisonWidget)
		{
			m_ComparisonWidget->DestroyViewport();
			//setting to visible somehow disables input on any widget when hitting back from StatisticsWidget. Same thing below
			//This ONLY happens with this widget and I have no idea why
			m_StatisticsWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			m_TopWidget = m_StatisticsWidget;
		}
		else if (m_TopWidget == m_UnitSelectionWidget)
		{
			if (m_bSelectingComparison)
			{
				m_TopWidget = m_StatisticsWidget;
				caller->SetVisibility(ESlateVisibility::Visible); //cancel out hiding just in this case
				m_StatisticsWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else
			{
				m_TopWidget = m_FactionOverviewWidget;
			}
		}

		m_bSelectingComparison = false;
	}
}

void AStatisticsOverviewHUD::OnComparisonButtonClick()
{
	if (m_TopWidget == m_StatisticsWidget)
	{
		m_StatisticsWidget->SetVisibility(ESlateVisibility::Collapsed); //go to unit selection
	}
	else if (m_TopWidget == m_ComparisonWidget)
	{
		m_ComparisonWidget->SetVisibility(ESlateVisibility::Collapsed);
		m_ComparisonWidget->DestroyViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Function call to OnComparisonButtonClick in unhandled state"));
	}

	m_bSelectingComparison = true;
	m_TopWidget = m_UnitSelectionWidget;
}
