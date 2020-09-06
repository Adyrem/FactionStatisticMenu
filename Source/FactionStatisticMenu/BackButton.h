// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "BackButton.generated.h"

/**
 * Back button hidding the current widget. Requires AStatisticsOverviewHUD to be the current HUD
 */
UCLASS()
class FACTIONSTATISTICMENU_API UBackButton : public UButton
{
	GENERATED_BODY()

public:
	UBackButton();

private:
	/**
	* Calls the fitting event in the hud class
	*/
	UFUNCTION()
		virtual void OnBackButtonClicked() const;
};
