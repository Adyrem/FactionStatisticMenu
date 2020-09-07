// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitSelectionWidget.h"
#include "UnitSelectionButton.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/ButtonSlot.h"
#include "Blueprint/WidgetTree.h"

void UUnitSelectionWidget::NativeConstruct()
{
	if (selection_button_type)
	{
		AddAssetMapToBox(building_assets, m_BuildingBox);
		AddAssetMapToBox(unit_assets, m_UnitBox);
		AddAssetMapToBox(heroes_assets, m_HeroesBox);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No buttons could be created, due to no type being provided in editor"));
	}
}

void UUnitSelectionWidget::AddAssetMapToBox(TMap<FString, FAssetWithAnimation> assets, UVerticalBox* vertical_box)
{
	for (auto& element : assets)
	{
		//cannot use CreateWidget for classes inheriting from UWidget
		UUnitSelectionButton* new_widget = WidgetTree->ConstructWidget<UUnitSelectionButton>(selection_button_type);
		UTextBlock* display_text;
		if (button_text_type)
		{
			display_text = WidgetTree->ConstructWidget<UTextBlock>(button_text_type);
		}
		else
		{
			 display_text = WidgetTree->ConstructWidget<UTextBlock>();
		}

		new_widget->AddChild(display_text);

		display_text->SetText(FText::FromString(element.Key));
		Cast<UButtonSlot>(display_text->Slot)->Padding = 25;

		new_widget->animation_sequence = element.Value.animation_sequence;
		new_widget->data_asset = element.Value.data_asset;

		vertical_box->AddChild(new_widget);
	}
}
