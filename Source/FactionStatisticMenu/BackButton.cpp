// Fill out your copyright notice in the Description page of Project Settings.


#include "BackButton.h"
#include "StatisticsOverviewHUD.h"

UBackButton::UBackButton()
{
	FScriptDelegate script_delegate;
	script_delegate.BindUFunction(this, FName("OnBackButtonClicked"));
	OnClicked.Add(script_delegate);
}

void UBackButton::OnBackButtonClicked() const
{
	const APlayerController* controller = GetWorld()->GetFirstPlayerController();
	if (controller)
	{
		AStatisticsOverviewHUD* statistics_hud = Cast<AStatisticsOverviewHUD>(controller->GetHUD());
		if (statistics_hud)
		{
			statistics_hud->OnBackButtonClick(GetParent());
		}
	}
}
