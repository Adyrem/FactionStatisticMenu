// Fill out your copyright notice in the Description page of Project Settings.


#include "FactionSelectionButton.h"
#include "Components/Button.h"
#include "StatisticsOverviewHUD.h"

void UFactionSelectionButton::NativeConstruct()
{
	Super::NativeConstruct();

	FScriptDelegate script_delegate;
	script_delegate.BindUFunction(this, FName("OnMainButtonClicked"));
	m_MainButton->OnClicked.AddUnique(script_delegate);
}

void UFactionSelectionButton::OnMainButtonClicked() const
{
	const APlayerController* controller = GetWorld()->GetFirstPlayerController();
	if (controller)
	{
		AStatisticsOverviewHUD* statistics_hud = Cast<AStatisticsOverviewHUD>(controller->GetHUD());
		if (statistics_hud)
		{
			statistics_hud->OnFactionButtonClick(this);
		}
	}
}
