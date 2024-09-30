// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldManager.h"
#include "Kismet/GameplayStatics.h"
#include "TerrainGenerator.h"

// Sets default values
AWorldManager::AWorldManager() :
    NoiseScale(0.05f),
    NoiseStrength(4.f),
    CaveNoiseScale(0.08f),
    CaveNoiseStrength(10.f),
    CaveThreshold(0.3f),
    TempScale(0.02f),
    MoistureScale(0.03f),
    BiomeSizeInChunks(4),
    BiomeMapSize(32)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    /*static ConstructorHelpers::FClassFinder<AChunk> 
    ChunkBPClass(TEXT("/Game/Blueprints/BP_Chunk"));
    ChunkClass = ChunkBPClass.Class;*/
}

void AWorldManager::UpdateWorld(FVector PlayerPosition)
{
    // Calculate current chunk coordinates based on player position
    FIntVector PlayerChunkCoords(
        FMath::FloorToInt(PlayerPosition.X / (ChunkSize * 100)),
        FMath::FloorToInt(PlayerPosition.Y / (ChunkSize * 100)),
        0);

    for (int32 X = -RenderDistance; X <= RenderDistance; X++)
    {
        for (int32 Y = -RenderDistance; Y <= RenderDistance; Y++)
        {
            FIntVector ChunkCoords = PlayerChunkCoords + FIntVector(X, Y, 0);

            if (!LoadedChunks.Contains(ChunkCoords))
            {
                LoadChunk(ChunkCoords);
            }
        }
    }
}

// Called when the game starts or when spawned
void AWorldManager::BeginPlay()
{
	Super::BeginPlay();
	
    // Generate the biome map
    GenerateBiomeMap(Seed);
}

// Called every frame
void AWorldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Get player location
	FVector PlayerPosition = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	UpdateWorld(PlayerPosition);
}

void AWorldManager::LoadChunk(FIntVector ChunkCoords)
{
    FVector ChunkPosition(ChunkCoords.X * ChunkSize * 100, ChunkCoords.Y * ChunkSize * 100, 0);

    //static ConstructorHelpers::FClassFinder<AChunk> ChunkBPClass(TEXT("/Game/Blueprints/BP_Chunk"));

    if (ChunkClass)
    {
        // Spawn the BP_Chunk
        AChunk* NewChunk = GetWorld()->SpawnActor<AChunk>(ChunkClass, ChunkPosition, FRotator::ZeroRotator);


        // Get the biome for this chunk
        EBiomeType Biome = GetBiomeForChunk(ChunkCoords);

        NewChunk->InitializeChunk(ChunkPosition, ChunkSize, 100);
        NewChunk->GenerateChunk(BlockMesh, Seed, NoiseScale, NoiseStrength, TempScale, MoistureScale, Biome);
        LoadedChunks.Add(ChunkCoords, NewChunk);
    }


    /////////////
    /*AChunk* NewChunk = GetWorld()->SpawnActor<AChunk>(ChunkPosition, FRotator::ZeroRotator);


    NewChunk->InitializeChunk(ChunkPosition, ChunkSize, 100);
    NewChunk->GenerateChunk(BlockMesh, Seed, NoiseScale, NoiseStrength, TempScale, MoistureScale);
    LoadedChunks.Add(ChunkCoords, NewChunk);*/
}

void AWorldManager::UnloadChunk(FIntVector ChunkCoords)
{
    if (AChunk** ChunkPtr = LoadedChunks.Find(ChunkCoords))
    {
        (*ChunkPtr)->Destroy();
        LoadedChunks.Remove(ChunkCoords);
    }
}

void AWorldManager::GenerateBiomeMap(int32 seed)
{
    // Resize biome map to hold values for each region (BiomeMapSize x BiomeMapSize)
    BiomeMap.SetNum(BiomeMapSize * BiomeMapSize);

    // Generate biomes for each region of chunks
    for (int32 X = 0; X < BiomeMapSize; X++)
    {
        for (int32 Y = 0; Y < BiomeMapSize; Y++)
        {

            float Temperature = TerrainGenerator::GenerateTemperatureNoise(X, Y, TempScale, NoiseStrength, seed);// Low resolution noise for biomes
            UE_LOG(LogTemp, Warning, TEXT("Temp Value: %f"), Temperature);
            float Moisture = TerrainGenerator::GenerateMoistureNoise(X, Y, MoistureScale, NoiseStrength, seed);
            UE_LOG(LogTemp, Warning, TEXT("Moist Value: %f"), Moisture);
            EBiomeType Biome = TerrainGenerator::DetermineBiome(Temperature, Moisture);

            // Store the biome in the biome map
            BiomeMap[X + Y * BiomeMapSize] = Biome;
        }
    }
}

EBiomeType AWorldManager::GetBiomeForChunk(FIntVector ChunkCoords)
{
    // Determine which "biome cell" the chunk belongs to
    int32 BiomeX = ChunkCoords.X / BiomeSizeInChunks;
    int32 BiomeY = ChunkCoords.Y / BiomeSizeInChunks;

    // Clamp to valid biome map range
    BiomeX = FMath::Clamp(BiomeX, 0, BiomeMapSize - 1);
    BiomeY = FMath::Clamp(BiomeY, 0, BiomeMapSize - 1);

    // Return the biome for this chunk's region
    return BiomeMap[BiomeX + BiomeY * BiomeMapSize];
}

