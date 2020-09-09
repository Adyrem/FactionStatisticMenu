// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScrollboxStatEntry.generated.h"

class UTextBlock;
struct FSlateColor;
struct FLinearColor;

/**
* Indicates if the comparison value is better or worse than the main value
*/
UENUM()
enum class EComparisonOptions
{
	equal = 0x00,
	less_than = 0x01,
	more_than = 0x02
};

/**
 * A single statistics entry containing a name, value and comparison value
 */
UCLASS()
class FACTIONSTATISTICMENU_API UScrollboxStatEntry : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSlateColor equal_color = FSlateColor(FLinearColor::White);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSlateColor less_than_color = FSlateColor(FLinearColor::Red);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSlateColor more_than_color = FSlateColor(FLinearColor::Green);

	/**
	* Applies the colors set in blueprint (or through c++) to the comparison textblock
	* @param ComparisonOption - Indicates >, =, < relationship
	*/
	void ApplyColorToComparison(const EComparisonOptions& comparison_option);

	void SetStatName(const FString& name);
	void SetStatValue(const FString& value);
	void SetComparisonValue(const FString& value);
	const FString& GetStatName() const;
	const FString& GetStatValue() const;
	FString GetComparisonValue() const; //cannot be const because ComparisonValue may not be present

private:
	UPROPERTY(meta = (BindWidget))
		UTextBlock* m_StatName;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* m_StatValue;
	UPROPERTY(meta = (BindWidgetOptional))
		UTextBlock* m_ComparisonValue;
};
