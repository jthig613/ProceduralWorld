// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BiomeType.h"

/**
 * 
 */
class PROCEDURALTERRAIN_API TerrainGenerator
{
public:
	//2d noise for height generation
	static float GenerateNoise(float X, float Y, float NoiseScale, float NoiseStrength, int32 Seed);

    // Generate temperature noise for biome determination
    static float GenerateTemperatureNoise(float X, float Y, float NoiseScale, float NoiseStrength, int32 Seed);

    // Generate moisture noise for biome determination
    static float GenerateMoistureNoise(float X, float Y, float NoiseScale, float NoiseStrength, int32 Seed);

    // Determine the biome type based on temperature and moisture
    static EBiomeType DetermineBiome(float Temperature, float Moisture);

	static float Generate3DNoise(float X, float Y, float Z, float NoiseScale, float NoiseStrength);
};
