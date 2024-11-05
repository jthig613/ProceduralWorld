// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldManager.h"
#include "Kismet/GameplayStatics.h"
#include "TerrainGenerator.h"

// Sets default values
AWorldManager::AWorldManager() :
    NoiseScale(0.05f),
    NoiseStrength(4.f),
    TempScale(0.02f),
    MoistureScale(0.03f),
    BiomeSizeInChunks(4),
    BiomeMapSize(32),
    GridSize(10)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    LastChunkUpdatePosition = FVector::ZeroVector;
    LastUpdateTime = 0.0f;

    MinDistanceToUpdateChunks = 500.0f; // Player needs to move at least 500 units
    UpdateInterval = 2.0f;
}


// Called when the game starts or when spawned
void AWorldManager::BeginPlay()
{
	Super::BeginPlay();
	
    // Generate the biome map
    GenerateBiomeMap(Seed);

    // Example of loading chunks in a 10x10 grid for demonstration
    for (int32 X = 0; X < GridSize; X++)
    {
        for (int32 Y = 0; Y < GridSize; Y++)
        {
            LoadChunk(FIntVector(X, Y, 0));
        }
    }
}

// Called every frame
void AWorldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Update PlayerPosition if the player moves, which affects LOD calculation
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        APawn* PlayerPawn = PlayerController->GetPawn();
        if (PlayerPawn)
        {
            PlayerPosition = PlayerPawn->GetActorLocation();
        }
    }
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
        // Calculate the LOD level based on player distance
        ELODLevel LODLevel = CalculateLODLevel(ChunkPosition);

        NewChunk->InitializeChunk(ChunkPosition, ChunkSize, 100);
        NewChunk->GenerateChunk(BlockMesh, Seed, NoiseScale, NoiseStrength, TempScale, MoistureScale, Biome, LODLevel);
        LoadedChunks.Add(ChunkCoords, NewChunk);
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

ELODLevel AWorldManager::CalculateLODLevel(FVector ChunkPosition)
{

    float Distance = FVector::Dist(PlayerPosition, ChunkPosition);

    if (Distance < LODHighDistance)
    {
        return ELODLevel::LOD_High;
    }
    else if (Distance < LODMediumDistance)
    {
        return ELODLevel::LOD_Medium;
    }
    else
    {
        return ELODLevel::LOD_Low;
    }
}

void AWorldManager::UpdateChunks(FVector playerposition)
{
    // Convert PlayerPosition into chunk coordinates
    FIntVector PlayerChunkCoord = GetChunkCoordinates(PlayerPosition);

    // Unload chunks that are too far from the player
    for (auto It = LoadedChunks.CreateIterator(); It; ++It)
    {
        FIntVector ChunkCoord = It->Key;
        FVector ChunkCenter = GetChunkCenter(ChunkCoord);

        float Distance = FVector::Dist(PlayerPosition, ChunkCenter);
        if (Distance > UnloadDistance)
        {
            // Unload chunk if it is beyond the unload distance
            UnloadChunk(ChunkCoord);
            It.RemoveCurrent();  // Remove from active chunk list
        }
    }

    // Load new chunks near the player
    for (int X = -GridSize; X <= GridSize; ++X)
    {
        for (int Y = -GridSize; Y <= GridSize; ++Y)
        {
            FIntVector ChunkCoord = PlayerChunkCoord + FIntVector(X, Y, 0);
            FVector ChunkCenter = GetChunkCenter(ChunkCoord);

            float Distance = FVector::Dist(PlayerPosition, ChunkCenter);
            if (Distance < LoadDistance && !LoadedChunks.Contains(ChunkCoord))
            {
                // Load chunk if it's within the load distance and not already loaded
                LoadChunk(ChunkCoord);
                LoadedChunks.Add(ChunkCoord);
            }
        }
    }
}

FVector AWorldManager::GetChunkCenter(FIntVector ChunkCoord)
{
    // Calculate the center of the chunk based on its coordinates
    return FVector(ChunkCoord.X * ChunkSize + ChunkSize / 2,
        ChunkCoord.Y * ChunkSize + ChunkSize / 2,
        0);
}

FIntVector AWorldManager::GetChunkCoordinates(FVector Position)
{
    // Convert world position to chunk coordinates
    return FIntVector(FMath::FloorToInt(Position.X / ChunkSize),
        FMath::FloorToInt(Position.Y / ChunkSize),
        FMath::FloorToInt(Position.Z / ChunkSize));
}

bool AWorldManager::ShouldUpdateChunks(FVector playerposition)
{
    // Get the current game time in seconds
    float CurrentTime = GetWorld()->GetTimeSeconds();

    // Calculate the distance between the player's current position and the last update position
    float Distance = FVector::Dist(PlayerPosition, LastChunkUpdatePosition);

    // Check if the player has moved far enough or enough time has passed
    if (Distance > MinDistanceToUpdateChunks || CurrentTime - LastUpdateTime > UpdateInterval)
    {
        // Update both the last update position and time
        LastChunkUpdatePosition = PlayerPosition;
        LastUpdateTime = CurrentTime;
        return true;  // Update chunks
    }

    return false;  // No update needed yet
}

void AWorldManager::UnloadChunk(FIntVector ChunkCoords)
{
    if (AChunk** ChunkPtr = LoadedChunks.Find(ChunkCoords))
    {
        (*ChunkPtr)->Destroy();
        LoadedChunks.Remove(ChunkCoords);
    }

    if (AActor* Chunk = LoadedChunks[ChunkCoords])
    {
        // Destroy chunk actors
        Chunk->Destroy();  
    }
}

