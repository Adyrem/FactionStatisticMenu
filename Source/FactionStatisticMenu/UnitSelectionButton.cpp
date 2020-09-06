// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitSelectionButton.h"
#include "StatisticsOverviewHUD.h"

UUnitSelectionButton::UUnitSelectionButton()
{
	FScriptDelegate ScriptDelegate;
	ScriptDelegate.BindUFunction(this, FName("OnButtonClicked"));
	OnClicked.Add(ScriptDelegate);
}

void UUnitSelectionButton::OnButtonClicked()
{
	APlayerController* Controller = GetWorld()->GetFirstPlayerController();
	if (Controller)
	{
		AStatisticsOverviewHUD* StatisticsHud = Cast<AStatisticsOverviewHUD>(Controller->GetHUD());
		StatisticsHud->OnUnitSelectionClick(this);
	}
}
