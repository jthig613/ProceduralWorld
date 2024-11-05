// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//We define our biome types here to be referenced later
UENUM(BlueprintType)
enum class EBiomeType : uint8
{
    Desert UMETA(DisplayName = "Desert"),
    Forest UMETA(DisplayName = "Forest"),
    Tundra UMETA(DisplayName = "Tundra"),
    Plains UMETA(DisplayName = "Plains")
};
