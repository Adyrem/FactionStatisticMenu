// Fill out your copyright notice in the Description page of Project Settings.

#include "StatisticsOverviewWidget.h"
#include "ScrollboxStatEntry.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Engine/TextureRenderTarget2D.h"
#include "StatisticsViewportCharacter.h"
#include "EntityDataAsset.h"
#include "UnitDataAsset.h"
#include "BuildingDataAsset.h"
#include "WeaponDataAsset.h"
#include "StatisticsViewportCharacter.h"
#include "StatisticsOverviewHUD.h"

void UStatisticsOverviewWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AddEntriesToScrollBox();

	if (main_data_asset)
	{
		m_EntityName->SetText(FText::FromString(main_data_asset->ui_name));
	}
	if (m_ComparisonEntityName && comparison_data_asset)
	{
		m_ComparisonEntityName->SetText(FText::FromString(comparison_data_asset->ui_name));
	}

	if (!m_bWasViewportOverwritten) //they can already be set through OverwriteViewport()
	{
		UStreamableRenderAsset* mesh = RetrieveMeshfromDataAsset(main_data_asset);
		if (mesh)
		{
			viewport_character = BuildViewport(mesh, main_render_target, main_spawnpoint, main_animation);
		}
		else
		{
			main_viewport->SetVisibility(ESlateVisibility::Collapsed); //it otherwise just shows a screenshot of the previous time it got called
		}
	}

	if (comparison_viewport)
	{
		UStreamableRenderAsset* comp_mesh = RetrieveMeshfromDataAsset(comparison_data_asset);
		if (comp_mesh)
		{
			m_ComparisonViewportCharacter = BuildViewport(comp_mesh, comparison_render_target, comparison_spawnpoint, comparison_animation);
		}
		else
		{
			comparison_viewport->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (viewport_character)
	{
		viewport_character->brotate_is_active = false;
		main_viewport->OnMouseButtonDownEvent.BindUFunction(this, FName("OnViewportMouseDown"));
	}
	if (m_ComparisonViewportCharacter)
	{
		m_ComparisonViewportCharacter->brotate_is_active = false;
		comparison_viewport->OnMouseButtonDownEvent.BindUFunction(this, FName("OnComparisonViewportMouseDown"));
	}

	if (m_CompareButton)
	{
		const APlayerController* controller = GetWorld()->GetFirstPlayerController();
		if (controller)
		{
			FScriptDelegate script_delegate;
			script_delegate.BindUFunction(controller->GetHUD(), FName("OnComparisonButtonClick"));
			m_CompareButton->OnClicked.Add(script_delegate);
		}
	}
}

UStreamableRenderAsset* UStatisticsOverviewWidget::RetrieveMeshfromDataAsset(const UEntityDataAsset* data_asset)
{
	const UUnitDataAsset* unit_asset = Cast<UUnitDataAsset>(data_asset);
	if (unit_asset && unit_asset->skeletal_mesh) //Rely on lazy evaluation for nullptr check
	{
		return unit_asset->skeletal_mesh;
	}

	const UBuildingDataAsset* building_asset = Cast<UBuildingDataAsset>(data_asset);
	if (building_asset && building_asset->static_or_skeletal_mesh)
	{
		return building_asset->static_or_skeletal_mesh;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No mesh found in Data Asset"));
		return nullptr;
	}
}

void UStatisticsOverviewWidget::AddEntriesToScrollBox()
{
	if (scroll_entry_widget && main_data_asset)
	{
		UScrollboxStatEntry* health = PrepareScrollBoxEntry("Health");
		if (health)
		{
			health->SetStatValue(FString::SanitizeFloat(main_data_asset->maximum_health));

			//assume that all other widgets will succeed because the first one passed aswell
			UScrollboxStatEntry* ressources_cost = PrepareScrollBoxEntry("Ressource Cost");
			ressources_cost->SetStatValue(FString::SanitizeFloat(main_data_asset->resources_cost));

			if (comparison_data_asset)
			{
				health->SetComparisonValue(FString::SanitizeFloat(comparison_data_asset->maximum_health));
				ressources_cost->SetComparisonValue(FString::SanitizeFloat(comparison_data_asset->resources_cost));

				ApplyColorBasedOnDifference(comparison_data_asset->maximum_health - main_data_asset->maximum_health, health);
				ApplyColorBasedOnDifference(comparison_data_asset->resources_cost - main_data_asset->resources_cost, ressources_cost);
			}
			else
			{
				health->SetComparisonValue("");
				ressources_cost->SetComparisonValue("");
			}

			m_StatsScrollBox->AddChild(health);
			m_StatsScrollBox->AddChild(ressources_cost);

			//add stats specific to the asset type
			const UUnitDataAsset* unit_asset = Cast<UUnitDataAsset>(main_data_asset);
			const UUnitDataAsset* comparison_unit_asset = Cast<UUnitDataAsset>(comparison_data_asset);
			if (unit_asset)
			{
				AddUnitStats(unit_asset, comparison_unit_asset);
				return;
			}

			const UBuildingDataAsset* building_asset = Cast<UBuildingDataAsset>(main_data_asset);
			const UBuildingDataAsset* comparison_building_asset = Cast<UBuildingDataAsset>(comparison_data_asset);
			if (building_asset)
			{
				AddBuildingStats(building_asset, comparison_building_asset);
				return;
			}
		}
	}
}

UScrollboxStatEntry* UStatisticsOverviewWidget::PrepareScrollBoxEntry(const FName& name)
{
	if (!scroll_entry_widget)
	{
		return nullptr;
	}
	UScrollboxStatEntry* return_widget = CreateWidget<UScrollboxStatEntry>(this, scroll_entry_widget, name);
	if (return_widget)
	{
		return_widget->SetStatName(name.ToString());
	}
	return return_widget;
}

void UStatisticsOverviewWidget::ApplyColorBasedOnDifference(const int32& difference, UScrollboxStatEntry* applied_to)
{
	if (benable_color_coded_values)
	{
		if (difference > 0)
		{
			applied_to->ApplyColorToComparison(EComparisonOptions::more_than);
		}
		else if (difference < 0)
		{
			applied_to->ApplyColorToComparison(EComparisonOptions::less_than);
		}
		else
		{
			applied_to->ApplyColorToComparison(EComparisonOptions::equal);
		}
	}
}

void UStatisticsOverviewWidget::AddUnitStats(const UUnitDataAsset* unit_asset, const UUnitDataAsset* comparison_asset)
{
	UScrollboxStatEntry* group_size = PrepareScrollBoxEntry("Group Size");
	group_size->SetStatValue(FString::FromInt(unit_asset->group_size));

	UScrollboxStatEntry* max_walk_speed = PrepareScrollBoxEntry("Walk Speed");
	max_walk_speed->SetStatValue(FString::SanitizeFloat(unit_asset->max_walk_speed));

	UScrollboxStatEntry* damage = PrepareScrollBoxEntry("Damage");

	UWeaponDataAsset* weapon_asset = unit_asset->weapon;
	if (weapon_asset)
	{
		damage->SetStatValue(FString::SanitizeFloat(weapon_asset->damage));

		if (comparison_asset)
		{
			const UWeaponDataAsset* comp_weapon = comparison_asset->weapon;
			if (comp_weapon)
			{
				damage->SetComparisonValue(FString::SanitizeFloat(comp_weapon->damage));
				ApplyColorBasedOnDifference(comp_weapon->damage - weapon_asset->damage, damage);
			}
		}

		m_StatsScrollBox->AddChild(damage);
	}

	if (comparison_asset)
	{
		group_size->SetComparisonValue(FString::FromInt(comparison_asset->group_size));
		max_walk_speed->SetComparisonValue(FString::SanitizeFloat(comparison_asset->max_walk_speed));

		ApplyColorBasedOnDifference(comparison_asset->group_size - unit_asset->group_size, group_size);
		ApplyColorBasedOnDifference(comparison_asset->max_walk_speed - unit_asset->max_walk_speed, max_walk_speed);
	}
	else
	{
		group_size->SetComparisonValue("");
		max_walk_speed->SetComparisonValue("");
		damage->SetComparisonValue("");
	}

	m_StatsScrollBox->AddChild(group_size);
	m_StatsScrollBox->AddChild(max_walk_speed);
}

void UStatisticsOverviewWidget::AddBuildingStats(const UBuildingDataAsset* building_asset, const UBuildingDataAsset* comparison_asset)
{

	if (comparison_asset)
	{

	}
}

void UStatisticsOverviewWidget::AddHeroStats(const UUnitDataAsset* unit_asset, const UUnitDataAsset* comparison_asset)
{

	if (comparison_asset)
	{

	}
}

AStatisticsViewportCharacter* UStatisticsOverviewWidget::BuildViewport(UStreamableRenderAsset* mesh, UTextureRenderTarget2D* render_target, const FVector& spawn_location, UAnimSequence* animation)
{
	AStatisticsViewportCharacter* character = GetWorld()->SpawnActor<AStatisticsViewportCharacter>(AStatisticsViewportCharacter::StaticClass(), spawn_location, FRotator(0, 0, 0));
	if (character && character->scene_capture)
	{
		character->GetCharacterMovement()->GravityScale = 1;
		character->scene_capture->TextureTarget = render_target;
		character->DisplayMesh(mesh);

		if (animation)
		{
			character->GetMesh()->PlayAnimation(animation, true);
		}
	}

	return character;
}

void UStatisticsOverviewWidget::OverwriteViewport(UImage* image_to_set, AStatisticsViewportCharacter* character_to_set)
{
	m_bWasViewportOverwritten = true;
	main_viewport->SetBrushFromMaterial(image_to_set->GetDynamicMaterial());
	if (viewport_character)
	{
		viewport_character->Destroy(); //get rid of the character if this was called multiple times
	}
	viewport_character = character_to_set;
}

void UStatisticsOverviewWidget::DestroyViewport()
{
	if (viewport_character && !m_bWasViewportOverwritten)
	{
		viewport_character->Destroy();
		main_viewport->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (m_ComparisonViewportCharacter)
	{
		m_ComparisonViewportCharacter->Destroy();
		comparison_viewport->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UStatisticsOverviewWidget::OnViewportMouseDown()
{
	if (viewport_character)
	{
		viewport_character->brotate_is_active = true;
		//need to send input events to the character
		GetWorld()->GetFirstPlayerController()->UnPossess();
		GetWorld()->GetFirstPlayerController()->Possess(viewport_character);
	}
}

//cant pass parameters to BindUFunction events, so this reduncancy is sadly needed in some way
void UStatisticsOverviewWidget::OnComparisonViewportMouseDown()
{
	if (m_ComparisonViewportCharacter)
	{
		m_ComparisonViewportCharacter->brotate_is_active = true;
		GetWorld()->GetFirstPlayerController()->UnPossess();
		GetWorld()->GetFirstPlayerController()->Possess(m_ComparisonViewportCharacter);
	}
}
