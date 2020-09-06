// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/TextBlock.h"
#include "ScrollboxStatEntry.h"

void UScrollboxStatEntry::SetStatName(const FString& name)
{
	m_StatName->SetText(FText::FromString(name));
}

void UScrollboxStatEntry::SetStatValue(const FString& value)
{
	m_StatValue->SetText(FText::FromString(value));
}

void UScrollboxStatEntry::SetComparisonValue(const FString& value)
{
	if (m_ComparisonValue)
	{
		m_ComparisonValue->SetText(FText::FromString(value));
	}
}

const FString& UScrollboxStatEntry::GetStatName() const
{
	return m_StatName->GetText().ToString();
}

const FString& UScrollboxStatEntry::GetStatValue() const
{
	return m_StatValue->GetText().ToString();
}

FString UScrollboxStatEntry::GetComparisonValue() const
{
	if (m_ComparisonValue)
	{
		return m_ComparisonValue->GetText().ToString();
	}
	return TEXT("");
}

void UScrollboxStatEntry::ApplyColorToComparison(const EComparisonOptions& comparison_option)
{
	switch (comparison_option)
	{
	case EComparisonOptions::less_than: m_ComparisonValue->SetColorAndOpacity(less_than_color);
		break;
	case EComparisonOptions::more_than: m_ComparisonValue->SetColorAndOpacity(more_than_color);
		break;
	default:
		m_ComparisonValue->SetColorAndOpacity(equal_color);
		break;
	}

}
