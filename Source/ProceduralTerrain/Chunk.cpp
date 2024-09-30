// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "Engine/InstancedStaticMesh.h"
#include "BiomeType.h"
#include "TerrainGenerator.h"


// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BlockMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BlockMesh"));
    RootComponent = BlockMeshComponent;
    
}

void AChunk::InitializeChunk(FVector InChunkPosition, int32 InChunkSize, int32 InBlockSize)
{
	ChunkPosition = InChunkPosition;
	ChunkSize = InChunkSize;
	BlockSize = InBlockSize;
}

void AChunk::GenerateChunk(UStaticMesh* BlockMesh, int32 Seed, float NoiseScale, float NoiseStrength, float TempScale, float MoistureScale, EBiomeType NewBiome)
{

    // Set block material based on biome
    UMaterialInterface* BlockMaterial = GetBiomeMaterial(NewBiome);
    BlockMeshComponent->SetStaticMesh(BlockMesh);
    // Simple noise-based generation for terrain height
    for (int32 X = 0; X < ChunkSize; X++)
    {
        for (int32 Y = 0; Y < ChunkSize; Y++)
        {
            // Use TerrainGenerator to generate a noise value for height
            float NoiseValue = TerrainGenerator::GenerateNoise(ChunkPosition.X + X, ChunkPosition.Y + Y, NoiseScale, NoiseStrength, Seed);
            UE_LOG(LogTemp, Warning, TEXT("Noise Value: %f"), NoiseValue);
            
            // Scale the noise value to get a usable height
            int32 Height = FMath::RoundToInt(NoiseValue * NoiseStrength);
            
            //int32 Height = FMath::RandRange(0, 10);

            // Ensure height is within a valid range (e.g., minimum height 1)
            Height = FMath::Clamp(Height, 1, ChunkSize);

            // Generate temperature and moisture values for biome determination
            //float Temperature = TerrainGenerator::GenerateTemperatureNoise(ChunkPosition.X + X, ChunkPosition.Y + Y, TempScale, NoiseStrength, Seed);
            //float Moisture = TerrainGenerator::GenerateMoistureNoise(ChunkPosition.X + X, ChunkPosition.Y + Y, MoistureScale, NoiseStrength, Seed);

            // Determine the biome based on temperature and moisture
            //EBiomeType Biome = TerrainGenerator::DetermineBiome(Temperature, Moisture);

            // Set block material based on biome
           //UMaterialInterface* BlockMaterial = GetBiomeMaterial(NewBiome);

            for (int32 Z = 0; Z < Height; Z++)
            {
                AddBlock(X, Y, Z, BlockMesh, BlockMaterial); // Pass the appropriate block mesh
            }
        }
    }
}

void AChunk::UpdateLOD(float PlayerDistance)
{
    // Adjust LOD based on player distance
    if (PlayerDistance > 5000.0f)
    {
        BlockMeshComponent->SetCullDistances(1000.0f, 10000.0f);
    }
    else
    {
        BlockMeshComponent->SetCullDistances(500.0f, 5000.0f);
    }
}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UMaterialInterface* AChunk::GetBiomeMaterial(EBiomeType Biome)
{
    switch (Biome)
    {
    case EBiomeType::Desert:
        return DesertMaterial;
    case EBiomeType::Forest:
        return ForestMaterial;
    case EBiomeType::Tundra:
        return TundraMaterial;
    case EBiomeType::Plains:
        return PlainsMaterial;
    default:
        return DefaultMaterial;
    }
}

void AChunk::AddBlock(int32 X, int32 Y, int32 Z, UStaticMesh* BlockMesh, UMaterialInterface* BlockMaterial)
{
    FVector BlockLocation = FVector(X * BlockSize, Y * BlockSize, Z * BlockSize);
    FTransform BlockTransform(FRotator(0, 0, 0), BlockLocation);

    // Create the instanced static mesh for this block
    int32 InstanceIndex = BlockMeshComponent->AddInstance(BlockTransform);

    // Set the material for this block instance based on the biome
    if (BlockMaterial)
    {
        BlockMeshComponent->SetMaterial(InstanceIndex, BlockMaterial);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("No material added"));
    }
    //BlockMeshComponent->AddInstance(BlockTransform);
}

