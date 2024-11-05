// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainGenerator.h"
#include "Math/UnrealMathUtility.h"



float TerrainGenerator::GenerateNoise(float X, float Y, float NoiseScale, float NoiseStrength, int32 Seed)
{
    // Use noise function to generate smooth terrain heights.
    return FMath::PerlinNoise2D(FVector2D(X * NoiseScale + (0.1 * Seed), Y * NoiseScale + (0.1 * Seed))) * NoiseStrength;
}

float TerrainGenerator::GenerateTemperatureNoise(float X, float Y, float NoiseScale, float NoiseStrength, int32 Seed)
{
    return FMath::PerlinNoise2D(FVector2D(X * NoiseScale + (0.1 * Seed), Y * NoiseScale + (0.1 * Seed))) * NoiseStrength;;
}

float TerrainGenerator::GenerateMoistureNoise(float X, float Y, float NoiseScale, float NoiseStrength, int32 Seed)
{
    return FMath::PerlinNoise2D(FVector2D(X * NoiseScale + (0.1 * Seed), Y * NoiseScale + (0.1 * Seed))) * NoiseStrength;;
}

EBiomeType TerrainGenerator::DetermineBiome(float Temperature, float Moisture)
{
    // Determine the biome type based on temperature and moisture.
    if (Temperature > 1.5f)
    {
        return EBiomeType::Desert;
    }
    else if (Temperature > 0.2f && Moisture > 0.5f)
    {
        return EBiomeType::Forest;
    }
    else if (Temperature < -0.1f)
    {
        return EBiomeType::Tundra;
    }
    else
    {
        return EBiomeType::Plains;
    }
}

float TerrainGenerator::Generate3DNoise(float X, float Y, float Z, float NoiseScale, float NoiseStrength)
{
    return FMath::PerlinNoise3D(FVector(X * NoiseScale + 0.1, Y * NoiseScale + 0.1, Z + 0.1 * NoiseScale)) * NoiseStrength;
}
