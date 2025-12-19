// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIEvents.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnToggleQuitMenu);

USTRUCT(BlueprintType)
struct FROZENBREAK_API FUIEvents
{
	GENERATED_BODY()

	FOnToggleQuitMenu OnToggleQuitMenu;
};
