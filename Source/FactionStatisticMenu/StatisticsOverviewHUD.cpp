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
			if (!m_ComparisonWidget)
			{
				m_ComparisonWidget = CreateWidget<UStatisticsOverviewWidget>(GetWorld(), m_ComparisonOverview);
				if (!m_ComparisonWidget)
				{
					return;
				}
				m_ComparisonWidget->AddToViewport();
			}

			m_ComparisonWidget->InitializeWidget(m_MainSelectedUnit->data_asset, m_MainSelectedUnit->animation_sequence, unit_selection_button->data_asset, unit_selection_button->animation_sequence);
			m_ComparisonWidget->OverwriteViewport(m_StatisticsWidget->GetOverwriteStructure());
			m_TopWidget = m_ComparisonWidget;
			m_ComparisonWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (unit_selection_button && m_StatisticsOverview)
	{
		if (!m_StatisticsWidget)
		{
			m_StatisticsWidget = CreateWidget<UStatisticsOverviewWidget>(GetWorld(), m_StatisticsOverview);
			if (!m_StatisticsWidget)
			{
				return;
			}
			m_StatisticsWidget->AddToViewport();
		}

		m_StatisticsWidget->InitializeWidget(unit_selection_button->data_asset, unit_selection_button->animation_sequence);
		m_TopWidget = m_StatisticsWidget;
		m_MainSelectedUnit = unit_selection_button;
		m_StatisticsWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void AStatisticsOverviewHUD::OnBackButtonClick(UWidget* caller)
{
	if (caller)
	{
		//setting to collapsed somehow disables input on any widget when hitting back from StatisticsWidget. 
		//When calling directly, this doesn't happen, even though it is a virtual method
		//This ONLY happens with this widget and I have no idea why
		//For all other cases, it can be called directly from caller
		if (!(m_TopWidget == m_StatisticsWidget || m_TopWidget == m_ComparisonWidget))
		{
			caller->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (m_TopWidget == m_StatisticsWidget)
		{
			m_StatisticsWidget->SetVisibility(ESlateVisibility::Collapsed);
			m_TopWidget = m_UnitSelectionWidget;
		}
		else if (m_TopWidget == m_ComparisonWidget)
		{
			m_ComparisonWidget->SetVisibility(ESlateVisibility::Collapsed);
			m_StatisticsWidget->SetVisibility(ESlateVisibility::Visible);
			m_TopWidget = m_StatisticsWidget;
		}
		else if (m_TopWidget == m_UnitSelectionWidget)
		{
			if (m_bSelectingComparison)
			{
				m_TopWidget = m_StatisticsWidget;
				caller->SetVisibility(ESlateVisibility::Visible); //cancel out hiding just in this case
				m_StatisticsWidget->SetVisibility(ESlateVisibility::Visible);
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
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Function call to OnComparisonButtonClick in unhandled state"));
	}

	m_bSelectingComparison = true;
	m_TopWidget = m_UnitSelectionWidget;
}
