// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define QUICK_LOG(text) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, text);
#define QUICK_LOG_UPDATE(text) GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, text);

DECLARE_LOG_CATEGORY_EXTERN(LogTwinStickShooter, Log, All);
