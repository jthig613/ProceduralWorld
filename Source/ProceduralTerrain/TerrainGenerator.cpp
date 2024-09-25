// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainGenerator.h"
#include "Math/UnrealMathUtility.h"


float TerrainGenerator::GenerateNoise(float X, float Y, float NoiseScale, float NoiseStrength)
{
    
    /*FastNoiseLite NoiseGenerator;
    NoiseGenerator.SetSeed(Seed);
    NoiseGenerator.SetFrequency(Scale);*/

    // Use noise function to generate smooth terrain heights.
    return FMath::PerlinNoise2D(FVector2D(X * NoiseScale + 0.1, Y * NoiseScale + 0.1)) * NoiseStrength;
}

float TerrainGenerator::Generate3DNoise(float X, float Y, float Z, float NoiseScale, float NoiseStrength)
{
    return FMath::PerlinNoise3D(FVector(X * NoiseScale + 0.1, Y * NoiseScale + 0.1, Z + 0.1 * NoiseScale)) * NoiseStrength;
}
