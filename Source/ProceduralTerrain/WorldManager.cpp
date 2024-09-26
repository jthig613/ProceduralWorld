// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWorldManager::AWorldManager() :
    NoiseScale(0.05f),
    NoiseStrength(4.f),
    CaveNoiseScale(0.08f),
    CaveNoiseStrength(10.f),
    CaveThreshold(0.3f),
    TempScale(0.02f),
    MoistureScale(0.03f)
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

        NewChunk->InitializeChunk(ChunkPosition, ChunkSize, 100);
        NewChunk->GenerateChunk(BlockMesh, Seed, NoiseScale, NoiseStrength, TempScale, MoistureScale);
        LoadedChunks.Add(ChunkCoords, NewChunk);
    }


    ///////////
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

