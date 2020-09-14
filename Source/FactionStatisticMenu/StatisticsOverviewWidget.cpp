// Fill out your copyright notice in the Description page of Project Settings.

#include "StatisticsOverviewWidget.h"
#include "ScrollboxStatEntry.h"
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

	//incase data assets and animations were set through blueprint.
	if (m_MainDataAsset)
	{
		InitializeWidget(m_MainDataAsset, m_MainAnimation, m_ComparisonDataAsset, m_ComparisonAnimation);
	}

	main_viewport->OnMouseButtonDownEvent.BindUFunction(this, FName("OnViewportMouseDown"));
	if (comparison_viewport)
	{
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

void UStatisticsOverviewWidget::InitializeWidget(UEntityDataAsset* main_data, UAnimSequence* main_animation_sequence)
{
	m_MainDataAsset = main_data;
	m_MainAnimation = main_animation_sequence;
	RebuildScrollboxEntries();

	if (m_MainDataAsset)
	{
		m_EntityName->SetText(FText::FromString(main_data->ui_name));

		if (!m_ViewportCharacter)
		{
			m_ViewportCharacter = BuildViewport(m_MainDataAsset, main_render_target, main_spawnpoint, m_MainAnimation);
		}
		else
		{
			m_ViewportCharacter->DisplayMesh(RetrieveMeshfromDataAsset(m_MainDataAsset));
			m_ViewportCharacter->GetMesh()->PlayAnimation(m_MainAnimation, true);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No main data asset provided, Widget could not be initialized correctly"));
	}
}

void UStatisticsOverviewWidget::InitializeWidget(UEntityDataAsset* main_data, UAnimSequence* main_animation_sequence, UEntityDataAsset* comp_data, UAnimSequence* comp_animation_sequence)
{
	m_ComparisonDataAsset = comp_data;
	m_ComparisonAnimation = comp_animation_sequence;
	InitializeWidget(main_data, main_animation_sequence); //watch out that this is called after setting the comp variables, because it may call methods that use them if set

	if (m_ComparisonDataAsset)
	{
		m_ComparisonEntityName->SetText(FText::FromString(m_ComparisonDataAsset->ui_name));
		if (comparison_viewport)
		{
			if (!m_ComparisonViewportCharacter)
			{
				m_ComparisonViewportCharacter = BuildViewport(m_ComparisonDataAsset, comparison_render_target, comparison_spawnpoint, m_ComparisonAnimation);
			}
			else
			{
				m_ComparisonViewportCharacter->DisplayMesh(RetrieveMeshfromDataAsset(m_ComparisonDataAsset));
				m_ComparisonViewportCharacter->GetMesh()->PlayAnimation(m_ComparisonAnimation, true);
			}
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

void UStatisticsOverviewWidget::AddEntriesToScrollBox(UScrollBox* scroll_box, const UEntityDataAsset* main, const UEntityDataAsset* comparison)
{
	if (scroll_entry_widget)
	{
		if (main)
		{
			UScrollboxStatEntry* health = PrepareScrollBoxEntry("Health");
			if (health)
			{
				health->SetStatValue(FString::SanitizeFloat(main->maximum_health));

				//assume that all other widgets will succeed because the first one passed aswell
				UScrollboxStatEntry* ressources_cost = PrepareScrollBoxEntry("Ressource Cost");
				ressources_cost->SetStatValue(FString::SanitizeFloat(main->resources_cost));

				if (comparison)
				{
					health->SetComparisonValue(FString::SanitizeFloat(comparison->maximum_health));
					ressources_cost->SetComparisonValue(FString::SanitizeFloat(comparison->resources_cost));

					ApplyColorBasedOnDifference(comparison->maximum_health - main->maximum_health, health);
					ApplyColorBasedOnDifference(comparison->resources_cost - main->resources_cost, ressources_cost);
				}
				else
				{
					health->SetComparisonValue("");
					ressources_cost->SetComparisonValue("");
				}

				scroll_box->AddChild(health);
				scroll_box->AddChild(ressources_cost);

				//add stats specific to the asset type
				const UUnitDataAsset* unit_asset = Cast<UUnitDataAsset>(main);
				const UUnitDataAsset* comparison_unit_asset = Cast<UUnitDataAsset>(comparison);
				if (unit_asset)
				{
					AddUnitStats(m_StatsScrollBox, unit_asset, comparison_unit_asset);
					return;
				}

				const UBuildingDataAsset* building_asset = Cast<UBuildingDataAsset>(main);
				const UBuildingDataAsset* comparison_building_asset = Cast<UBuildingDataAsset>(comparison);
				if (building_asset)
				{
					AddBuildingStats(m_StatsScrollBox, building_asset, comparison_building_asset);
					return;
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Tried to add an entry to the stat scroll box, but no entry widget type was provided"));
	}
}

void UStatisticsOverviewWidget::RebuildScrollboxEntries()
{
	for (TTuple<FName, UScrollboxStatEntry*> entry : m_ScrollEntries)
	{
		entry.Value->SetVisibility(ESlateVisibility::Collapsed);
	}

	AddEntriesToScrollBox(m_StatsScrollBox, m_MainDataAsset, m_ComparisonDataAsset);
}

UScrollboxStatEntry* UStatisticsOverviewWidget::PrepareScrollBoxEntry(const FName& name)
{
	UScrollboxStatEntry* return_widget;
	if (m_ScrollEntries.Find(name))
	{
		return_widget = *m_ScrollEntries.Find(name);
		return_widget->SetVisibility(ESlateVisibility::Visible);
		return_widget->SetStatValue("");
		return_widget->SetComparisonValue("");
		return_widget->ApplyColorToComparison(EComparisonOptions::equal);
	}
	else
	{
		if (!scroll_entry_widget)
		{
			return nullptr;
		}
		return_widget = CreateWidget<UScrollboxStatEntry>(this, scroll_entry_widget, name);
		m_ScrollEntries.Add(name, return_widget);
	}

	if (return_widget)
	{
		return_widget->SetStatName(name.ToString());
	}
	return return_widget;
}

void UStatisticsOverviewWidget::ApplyColorBasedOnDifference(const int32& difference, UScrollboxStatEntry* applied_to) const
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
	else
	{
		applied_to->ApplyColorToComparison(EComparisonOptions::equal); //if the coloring is disabled, still set it to the equal color if another is currently set
	}
}

void UStatisticsOverviewWidget::AddUnitStats(UScrollBox* scroll_box, const UUnitDataAsset* unit_asset, const UUnitDataAsset* comparison_asset)
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

		scroll_box->AddChild(damage);
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

	scroll_box->AddChild(group_size);
	scroll_box->AddChild(max_walk_speed);
}

void UStatisticsOverviewWidget::AddBuildingStats(UScrollBox* scroll_box, const UBuildingDataAsset* building_asset, const UBuildingDataAsset* comparison_asset)
{

	if (comparison_asset)
	{

	}
}

void UStatisticsOverviewWidget::AddHeroStats(UScrollBox* scroll_box, const UUnitDataAsset* unit_asset, const UUnitDataAsset* comparison_asset)
{

	if (comparison_asset)
	{

	}
}

AStatisticsViewportCharacter* UStatisticsOverviewWidget::BuildViewport(UEntityDataAsset* data_asset, UTextureRenderTarget2D* render_target, const FVector& spawn_location, UAnimSequence* animation) const
{
	//spawn it even if no mesh or render target is available
	AStatisticsViewportCharacter* character = GetWorld()->SpawnActor<AStatisticsViewportCharacter>(AStatisticsViewportCharacter::StaticClass(), spawn_location, FRotator(0, 0, 0));

	UStreamableRenderAsset* mesh = RetrieveMeshfromDataAsset(data_asset);
	if (mesh)
	{
		if (character)
		{
			character->InitializeCharacter(render_target);
			character->DisplayMesh(mesh);

			if (animation)
			{
				character->GetMesh()->PlayAnimation(animation, true);
			}
			character->brotate_is_active = false;
		}
	}

	return character;
}

void UStatisticsOverviewWidget::OverwriteViewport(const TTuple<UImage*, AStatisticsViewportCharacter*> overwrite_from)
{
	UImage* from_image = overwrite_from.Get<0>();
	AStatisticsViewportCharacter* from_character = overwrite_from.Get<1>();

	if (m_bViewportOverwritten == false) //only make this possible once, as the overwritten character cannot be deleted
	{
		m_ViewportCharacter->Destroy();
	}
	main_viewport->SetBrushFromMaterial(from_image->GetDynamicMaterial());
	m_ViewportCharacter = from_character;
	
	m_bViewportOverwritten = true;
}

const TTuple<UImage*, AStatisticsViewportCharacter*> UStatisticsOverviewWidget::GetOverwriteStructure() const
{
	return TTuple<UImage*, AStatisticsViewportCharacter*>(main_viewport, m_ViewportCharacter);
}

void UStatisticsOverviewWidget::OnViewportMouseDown()
{
	if (m_ViewportCharacter)
	{
		m_ViewportCharacter->brotate_is_active = true;
		//need to send input events to the character
		GetWorld()->GetFirstPlayerController()->UnPossess();
		GetWorld()->GetFirstPlayerController()->Possess(m_ViewportCharacter);
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
