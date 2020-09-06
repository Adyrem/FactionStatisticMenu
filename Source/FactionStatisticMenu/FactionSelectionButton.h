// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Components/TextBlock.h"
#include "FactionSelectionButton.generated.h"

class UButton;

/**
 * A Widget containing an image and a faction name. When pressed, shows the attached widget
 */
UCLASS()
class FACTIONSTATISTICMENU_API UFactionSelectionButton : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UTexture2D* faction_image;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString faction_name;
	/** This will be shown when the MainButton of this widget is pressed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UUserWidget> called_widget;

private:
	/**
	* Calls the function in the hud which shows the next widget
	*/
	UFUNCTION()
		void OnMainButtonClicked() const;

	UPROPERTY(meta = (BindWidget))
		UButton* m_MainButton;
};
