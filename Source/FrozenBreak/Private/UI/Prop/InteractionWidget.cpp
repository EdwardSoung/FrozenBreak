// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Prop/InteractionWidget.h"
#include "Components/TextBlock.h"
#include "Data/InteractionTextData.h"

void UInteractionWidget::UpdateInteraction(EPropType InType, EInteractKeyType InKey)
{
	if (TextData)
	{
		FText* TypeText = TextData->TypeText.Find(InType);
		InteractionType->SetText(*TypeText);
		
		FText* KeyText = TextData->KeyText.Find(InKey);
		//FText FormatPattern = FText::FromString(TEXT("[{0}]"));
		InteractionKey->SetText(*KeyText);

	}
	else
	{
		//꺼줄지..?
	}
}
