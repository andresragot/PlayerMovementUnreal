// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class AUREISTAR_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

//Components
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement) UAureiCharacterMovementComponent* AureiCharacterMovementComponent;

private:
	//Spring Arm Component para que la cámara persiga detras al player
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta=(AllowPrivateAccess = "true")) class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true")) class UCameraComponent* FollowCamera;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input) float TurnRateGamepad;

public:
	bool bPressedAureiJump;

	UPROPERTY(EditAnyWhere, Category = "PlayerCharacter Properties", BlueprintReadWrite) float MaxElevationFly;
	UPROPERTY(EditAnyWhere, Category = "PlayerCharacter Properties", BlueprintReadWrite) float DashDistance = 2000;
	UPROPERTY(EditAnyWhere, Category = "PlayerCharacter Properties", BlueprintReadWrite) float TimeGroundPound = 0.5f;
	UPROPERTY(EditAnyWhere, Category = "PlayerCharacter Properties", BlueprintReadWrite) float WallRunDeceleration = 2;


public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
 
	virtual void Jump() override;
	virtual void StopJumping() override;

//Input
public:
	//Funcion para activar el wall ride
	UFUNCTION(BlueprintCallable)
		void StartWallRide();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) bool bIsGravityEnabled;

	bool bCanGoUp;

private:
	//Llamado para adelante/atras inputs
	void MoveForward(float InputAxis);

	//Llamado para delante/derecha inputs
	void MoveRight(float InputAxis);

	//Llamado para arriba/abajo inputs
	void MoveUp(float InputAxis);

	//Vuela
	void Fly();

	//Esta funcion verifica si hemos llegado al limite de cuanto se puede elevar.
	void RaiseCharacter();

	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	//Variable que indica 
	bool bCanWallRide;

	bool CheckNearbyWall(FHitResult& HitResult) const;

	void Dashing();

	void GroundPound();

	void WallRun();

	void StopWallRun();

	bool IsWallNearby();

	void WallRunning();

private:
	double InitialWallRunSpeed;
	FVector WallNormal;
	float AngleWall;
	bool IsWallRunning = false;

//APawn interface
protected:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	UFUNCTION(BlueprintPure) FORCEINLINE UAureiCharacterMovementComponent* GetAureiCharacterMovement() const { return AureiCharacterMovementComponent; }

	FCollisionQueryParams GetIgnoreCharacterParams() const;

public:
	UPROPERTY(EditAnyWhere, Category = "PlayerCharacter Properties", BlueprintReadWrite)
	float MaxSpeedRun;

	UPROPERTY(EditAnyWhere, Category = "PlayerCharacter Properties", BlueprintReadWrite)
	float NormalSpeedRun;

protected:
	//Le pone los valores al personaje de velocidad a los de correr
	void BeginSprint();

	//Le pone los valores al personaje de velocidad a los de default
	void EndSprint();

	//Se agacha
	void BeginCrouch();

	//Se para
	void EndCrouch();
};
