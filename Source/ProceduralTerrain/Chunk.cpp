// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "Engine/InstancedStaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "BiomeType.h"
#include "Math/UnrealMathUtility.h"
#include "TerrainGenerator.h"


// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BlockMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BlockMesh"));
    RootComponent = BlockMeshComponent;
    
}

void AChunk::InitializeChunk(FVector InChunkPosition, int32 InChunkSize, int32 InBlockSize)
{
	ChunkPosition = InChunkPosition;
	ChunkSize = InChunkSize;
	BlockSize = InBlockSize;
}

void AChunk::GenerateChunk(UStaticMesh* BlockMesh, int32 Seed, float NoiseScale, float NoiseStrength, float TempScale, float MoistureScale, EBiomeType NewBiome, ELODLevel LODLevel)
{

    // Set block material based on biome
    UMaterialInterface* BlockMaterial = GetBiomeMaterial(NewBiome);
    float NewStrength = GetBiomeNoiseStrength(NewBiome);

    int32 LODChunkSize = ChunkSize;
    int32 BlockSpacing = 1; // Default spacing between blocks for high LOD

    BlockMeshComponent->SetStaticMesh(BlockMesh);

    // Adjust chunk resolution based on LOD level
    switch (LODLevel)
    {
    case ELODLevel::LOD_High:
        LODChunkSize = ChunkSize;  // Full detail
        BlockSpacing = 1;
        break;
    case ELODLevel::LOD_Medium:
        LODChunkSize = ChunkSize / 2;  // Half the detail
        BlockSpacing = 2;
        break;
    case ELODLevel::LOD_Low:
        LODChunkSize = ChunkSize / 4;  // Very simplified
        BlockSpacing = 4;
        break;
    }

    // Simple noise-based generation for terrain height
    for (int32 X = 0; X < ChunkSize; X++)
    {
        for (int32 Y = 0; Y < ChunkSize; Y++)
        {

            ////////////////////////


            // Use TerrainGenerator to generate a noise value for height
            float NoiseValue = TerrainGenerator::GenerateNoise(ChunkPosition.X + X, ChunkPosition.Y + Y, NoiseScale, NewStrength, Seed);
            UE_LOG(LogTemp, Warning, TEXT("Noise Value: %f"), NoiseValue);

            // Scale the noise value to get a usable height
            int32 Height = FMath::RoundToInt(NoiseValue * NewStrength);



            
            //int32 Height = FMath::RandRange(0, 10);

            // Ensure height is within a valid range (e.g., minimum height 1)
            Height = FMath::Clamp(Height, 1, ChunkSize);

            for (int32 Z = 0; Z < Height; Z++)
            {
               AddBlock(X, Y, Z, BlockMesh, BlockMaterial, LODLevel); // Pass the appropriate block mesh
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

    ////// LOD
    //FVector PlayerPosition = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
    //auto Distance = PlayerPosition - GetActorLocation();
    //float DLength = Distance.Length();
    //UpdateLOD(DLength);
    ////// LOD




    
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

float AChunk::GetBiomeNoiseStrength(EBiomeType Biome)
{
    switch (Biome)
    {
    case EBiomeType::Desert:
        return 4.f;
    case EBiomeType::Forest:
        return 2.f;
    case EBiomeType::Tundra:
        return 7.f;
    case EBiomeType::Plains:
        return 3.f;
    default:
        return 1.f;
    }
}

void AChunk::AddBlock(int32 X, int32 Y, int32 Z, UStaticMesh* BlockMesh, UMaterialInterface* BlockMaterial, ELODLevel LODLevel)
{
    FVector BlockLocation = FVector(X * BlockSize, Y * BlockSize, Z * BlockSize);
    FTransform BlockTransform(FRotator(0, 0, 0), BlockLocation);

    // Adjust the scale of blocks for lower LOD levels
    FVector BlockScale = FVector(1.0f, 1.0f, 1.0f);
    switch (LODLevel)
    {
    case ELODLevel::LOD_High:
        BlockScale = FVector(1.0f);  // Normal scale
        break;
    case ELODLevel::LOD_Medium:
        BlockScale = FVector(2.0f);  // Larger blocks
        break;
    case ELODLevel::LOD_Low:
        BlockScale = FVector(4.0f);  // Even larger blocks
        break;
    }

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
}

