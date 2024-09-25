// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "Engine/InstancedStaticMesh.h"
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

void AChunk::GenerateChunk(UStaticMesh* BlockMesh, int32 Seed, float NoiseScale, float NoiseStrength)
{
    BlockMeshComponent->SetStaticMesh(BlockMesh);
    // Simple noise-based generation for terrain height
    for (int32 X = 0; X < ChunkSize; X++)
    {
        for (int32 Y = 0; Y < ChunkSize; Y++)
        {
            // Use TerrainGenerator to generate a noise value for height
            float NoiseValue = TerrainGenerator::GenerateNoise(ChunkPosition.X + X, ChunkPosition.Y + Y, NoiseScale, NoiseStrength);
            UE_LOG(LogTemp, Warning, TEXT("Noise Value: %f"), NoiseValue);
            
            // Scale the noise value to get a usable height
            int32 Height = FMath::RoundToInt(NoiseValue * NoiseStrength);
            
            //int32 Height = FMath::RandRange(0, 10);

            // Ensure height is within a valid range (e.g., minimum height 1)
            Height = FMath::Clamp(Height, 1, ChunkSize);

            for (int32 Z = 0; Z < Height; Z++)
            {
                AddBlock(X, Y, Z, BlockMesh); // Pass the appropriate block mesh
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

void AChunk::AddBlock(int32 X, int32 Y, int32 Z, UStaticMesh* BlockMesh)
{
    FVector BlockLocation = FVector(X * BlockSize, Y * BlockSize, Z * BlockSize);
    FTransform BlockTransform(FRotator(0, 0, 0), BlockLocation);
    BlockMeshComponent->AddInstance(BlockTransform);
}

