// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ActorComponent.h"
#include "GameFramework/PlayerInput.h"
#include "Engine/DataTable.h"
//#include "EnhancedInputSubsystems.h" Para cuando Unreal lo arregle.








// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    
}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
    Super::BeginPlay();
    
    
    
    
}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    
    
}







