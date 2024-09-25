// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PROCEDURALTERRAIN_API TerrainGenerator
{
public:
	static float GenerateNoise(float X, float Y, float NoiseScale, float NoiseStrength, int32 Seed);

	static float Generate3DNoise(float X, float Y, float Z, float NoiseScale, float NoiseStrength);
};
