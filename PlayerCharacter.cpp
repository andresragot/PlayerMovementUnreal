// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "AureiCharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UAureiCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)) {
	
	AureiCharacterMovementComponent = Cast<UAureiCharacterMovementComponent>(GetCharacterMovement());
	AureiCharacterMovementComponent->SetIsReplicated(true);

	TurnRateGamepad = 50.f;

	bUseControllerRotationPitch = bUseControllerRotationYaw = bUseControllerRotationRoll = false;

	//Configure character movement
	AureiCharacterMovementComponent->bOrientRotationToMovement = true;
	AureiCharacterMovementComponent->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//Definimos la posicion del mesh y la rotacion
	//GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FQuat(FRotator(0.0f, -90.0f, 0.0f)));

	//Setting las variables de la clase del movimiento del personaje
	AureiCharacterMovementComponent->bUseControllerDesiredRotation = true;
	AureiCharacterMovementComponent->bIgnoreBaseRotation = true;

	bCanWallRide = false;
}


// Sets default values
//APlayerCharacter::APlayerCharacter()
//{
//	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
//
//	//Instantiando los componentes de la clase
//
//	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
//	FollowCamera = CreateDefaultSubobject < UCameraComponent>(TEXT("CameraComp"));
//
//	//Definimos la posicion del mesh y la rotacion
//	//GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FQuat(FRotator(0.0f, -90.0f, 0.0f)));
//
//	//Añadimos los componentes de la clase a el mesh
//
//	CameraBoom->SetupAttachment(GetMesh());
//	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
//
//	//Setting las variables de la clase del spring arm
//	CameraBoom->bUsePawnControlRotation = true;
//
//	//Setting las variables de la clase del movimiento del personaje
//	AureiCharacterMovementComponent->bOrientRotationToMovement = true;
//	AureiCharacterMovementComponent->bUseControllerDesiredRotation = true;
//	AureiCharacterMovementComponent->bIgnoreBaseRotation = true;
//
//	bCanWallRide = false;
//
//}



// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	bIsGravityEnabled = true;

	bCanGoUp = false;
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//Llama a la funcion RaiseCharacter en cada fotograma.
	RaiseCharacter();

	//Llama a la funcion de WallRunning
	//WallRunning();

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &APlayerCharacter::MoveUp);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APlayerCharacter::EndCrouch);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::BeginSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::EndSprint);
	PlayerInputComponent->BindAction("Fly", IE_Pressed, this, &APlayerCharacter::Fly);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerCharacter::Dashing);
	PlayerInputComponent->BindAction("GroundP", IE_Pressed, this, &APlayerCharacter::GroundPound);
	PlayerInputComponent->BindAction("WallRun", IE_Pressed, this, &APlayerCharacter::WallRun);


	PlayerInputComponent->BindTouch(IE_Pressed, this, &APlayerCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &APlayerCharacter::TouchStopped);
}

void APlayerCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location) {
	Jump();
}

void APlayerCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location) {
	StopJumping();
}

void APlayerCharacter::Jump() {
	bPressedAureiJump = true;

	Super::Jump();

	bPressedJump = false;
}

void APlayerCharacter::StopJumping() {
	bPressedAureiJump = false;
	Super::StopJumping();
}

void APlayerCharacter::MoveForward(float AxisValue) {
	if ((Controller != nullptr) && (AxisValue != 0.0f)) {
		
		//Buscar que dirección es hacia adelante
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		//Obtener el vector hacia adelante
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		//Añadir movimiento a esa dirección
		AddMovementInput(Direction, AxisValue);
	}
}

void APlayerCharacter::MoveRight(float AxisValue) {
	if (Controller != nullptr && AxisValue != 0.0f) {
		
		//Buscar que dirección es hacia la derecha
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		//Obtener el vector hacia la derecha 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		//Añadir movimiento a esa direccion
		AddMovementInput(Direction, AxisValue);
	}
}

void APlayerCharacter::MoveUp(float AxisValue) {
	if (bCanGoUp) {
		if (Controller != nullptr && AxisValue != 0.0f) {

			//Buscar que direccion es hacia arriba
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			//Obtener el vector hacia arriba
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Z);

			//Añadir movimiento a esa direccion
			AddMovementInput(Direction, AxisValue);	
		}
	}
}

void APlayerCharacter::BeginSprint() {
	AureiCharacterMovementComponent->MaxWalkSpeed = MaxSpeedRun;
}

void APlayerCharacter::EndSprint() {
	AureiCharacterMovementComponent->MaxWalkSpeed = NormalSpeedRun;
}

void APlayerCharacter::BeginCrouch() {
	Crouch();
}

void APlayerCharacter::EndCrouch() {
	UnCrouch();
}

void APlayerCharacter::Fly() {
	bIsGravityEnabled = !bIsGravityEnabled;

	//Si la gravedad se ha habilitado, restaura la gravedad del personaje
	if (bIsGravityEnabled) {
		AureiCharacterMovementComponent->GravityScale = 1.0f;
		AureiCharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	else {
		AureiCharacterMovementComponent->GravityScale = 0.0f;
		AureiCharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
	}
}

void APlayerCharacter::Dashing() {
	const FVector UpDir = this->GetActorUpVector();
	LaunchCharacter(UpDir * DashDistance, true, true);
}

void APlayerCharacter::GroundPound() {
	if (AureiCharacterMovementComponent->IsFlying() || AureiCharacterMovementComponent->IsFalling()) {
		FVector characterLocation = GetActorLocation();

		FVector start = characterLocation;
		FVector end = characterLocation - FVector::UpVector * 10000000;
		FHitResult hitResult;
		GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, GetIgnoreCharacterParams());

		if (hitResult.bBlockingHit) {
			float DistanceToGround = hitResult.Distance;
			float GroundPoundSpeed = DistanceToGround / TimeGroundPound;

			FVector GroundPoundVelocity = FVector(0, 0, -GroundPoundSpeed);
			LaunchCharacter(GroundPoundVelocity, true, true);
		}
	}
}

void APlayerCharacter::RaiseCharacter() {
	
	//Si la gravedad no está activada estamos en este caso
	if (!bIsGravityEnabled) {

		//Establece una altura máxima a la que se elevará el personaje
		float maxElevation = MaxElevationFly;

		//Obtiene la posicion del personaje
		FVector characterLocation = GetActorLocation();

		//Encuentra la posición del suelo usando un trazado de línea hacia abajo desde la posicion actual del personaje 
		FVector start = characterLocation;
		FVector end = characterLocation - FVector::UpVector * maxElevation;
		FHitResult hitResult;
		GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, GetIgnoreCharacterParams());

		if (hitResult.bBlockingHit) {
			//Calcula la distancia desde el centro del personaje hasta el suelo
			float distanceToGround = characterLocation.Z - hitResult.ImpactPoint.Z;
			
			if (distanceToGround <= maxElevation) {
				
				//Si la distancia al suelo es menor que la altura máxima, el personaje puede elevarse.
				bCanGoUp = true;
				AureiCharacterMovementComponent->bOrientRotationToMovement = false;
				AureiCharacterMovementComponent->AirControl = 1.0f;
			}
			else {
				//Si llego a el limite se cae al suelo y entra en estado ground
				Fly();
			}
		}
		else {
			//Si no se detecta colision con el suelo
			UE_LOG(LogTemp, Warning, TEXT("No se detectó collision con el suelo."))
			Fly();
		}
	} 
	else {
		bCanGoUp = false;
		AureiCharacterMovementComponent->bOrientRotationToMovement = true;
		AureiCharacterMovementComponent->AirControl = 0.0f;
	}

}

void APlayerCharacter::StartWallRide() {
	
	//AureiCharacterMovementComponent->TryWallRun();

	////Verificar si el personaje está cerca de la pared
	//FHitResult HitResult;
	//if (CheckNearbyWall(HitResult)) {

	//	//Calcular el ángulo entre la pared y la direccion de movimiento del personaje
	//	FVector CharacterForward = GetActorForwardVector();
	//	FVector WallNormal = HitResult.Normal;
	//	float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CharacterForward, WallNormal)));

	//	//Calcular la velocidad inicial (Velocidad de ascenso) basada en el ángulo de la pared
	//	float MaxAngle = 90.0f; //Ángulo máximo para una curvatura máxima
	//	float MinAngle = 0.0f; //Ángulo mínimo para una curvatura mínima
	//	float MaxSpeed = 1000.0f; //Velocidad máxima de ascenso
	//	float MinSpeed = 100.0f; //Velocidad mínima de ascenso
	//	float SpeedRange = MaxSpeed - MinSpeed;
	//	float SpeedFactor = FMath::Clamp((AngleDegrees - MinAngle) / (MaxAngle - MinAngle), 0.0f, 1.0f);
	//	float InitialSpeed = MinSpeed + SpeedFactor * SpeedRange;

	//	//Aplicar la curvatura del recorrido utilizando la fórmula de movimiento parabólico
	//	const float GRAVITY = AureiCharacterMovementComponent->GetGravityZ();
	//	FVector WallRideDirection = FVector::CrossProduct(WallNormal, CharacterForward); //Direccion del Wall ride
	//	float Time = 0.0f;
	//	float DeltaTime = GetWorld()->GetDeltaSeconds();
	//	FVector CharacterLocation = GetActorLocation();
	//	FVector CharacterVelocity = InitialSpeed * WallRideDirection;
	//	while (bCanWallRide)//mientras esté activo el Wall Ride
	//	{
	//		CharacterLocation += CharacterVelocity * DeltaTime;
	//		CharacterVelocity.Z -= GRAVITY * DeltaTime;
	//		Time += DeltaTime;

	//		SetActorLocation(CharacterLocation);
	//	}
	//}
}

void APlayerCharacter::WallRun() {
	//Comprueba si el personaje tiene una pared cerca
	if (!IsWallNearby()) {
		return;
	}

	//Guarda la velocidad inicial para usarla en el cálculo del wall run
	InitialWallRunSpeed = GetVelocity().Size();

	//Comianza el WallRun
	IsWallRunning = true;
	AureiCharacterMovementComponent->SetMovementMode(MOVE_Walking);
	WallRunning();
	//AureiCharacterMovementComponent->GravityScale = 0.0f;

}

void APlayerCharacter::StopWallRun() {
	//Restaura la gravedad y detiene el wall run
	AureiCharacterMovementComponent->GravityScale = 1.0f;
	IsWallRunning = false;
}

bool APlayerCharacter::IsWallNearby() {
	//Realiza una comprobación para ver si hay una pared cerca
	FVector Start = GetActorLocation();
	FVector LeftEnd = Start - (GetActorRightVector() * 1000);
	FVector RightEnd = Start + GetActorRightVector() * 1000;
	auto Params = GetIgnoreCharacterParams();
	FHitResult WallHit;

	//Cast izquierdo
	GetWorld()->LineTraceSingleByProfile(WallHit, Start, LeftEnd, "BlockAll", Params);
	if (WallHit.IsValidBlockingHit() /*&& (GetVelocity() | WallHit.Normal) < 0*/)
	{
		UE_LOG(LogTemp, Warning, TEXT("Entramos left"));

		AureiCharacterMovementComponent->Safe_bWallRunIsRight = false;
		WallNormal = WallHit.ImpactNormal;
		AngleWall = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorRightVector(), WallNormal)));

	}
	else {
		//Cast derecho
		GetWorld()->LineTraceSingleByProfile(WallHit, Start, RightEnd, "BlockAll", Params);
		if (WallHit.IsValidBlockingHit() /*&& (GetVelocity() | WallHit.Normal) < 0*/) {

			UE_LOG(LogTemp, Warning, TEXT("Entramos right"));

			AureiCharacterMovementComponent->Safe_bWallRunIsRight = true;
			WallNormal = WallHit.ImpactNormal;
			AngleWall = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorRightVector(), WallNormal)));
			AngleWall -= 180;
			AngleWall *= -1;

		}
		else {
			return false;
		}
	}

	AngleWall += 45;

	UE_LOG(LogTemp, Warning, TEXT("Angulo Pared: %s"), *WallNormal.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Cual pared de ambas es: %s"), AureiCharacterMovementComponent->Safe_bWallRunIsRight ? TEXT("True") : TEXT("False"));

	UE_LOG(LogTemp, Warning, TEXT("Angulo pared: %f"), AngleWall);

	//Pasó las condiciones 
	return true;
}

void APlayerCharacter::WallRunning() {
	if (IsWallRunning) {



		AngleWall -= 25;
		AngleWall /= 35;
		AngleWall *= -1;
		AngleWall /= 2;
		AngleWall += 0.1f;

		AureiCharacterMovementComponent->GravityScale = AngleWall;

		AureiCharacterMovementComponent->Velocity +=  GetActorRightVector() * 10;

		UE_LOG(LogTemp, Warning, TEXT("Estamos WallRunning"));

		UE_LOG(LogTemp, Error, TEXT("Vector Velocity: %s"), *AureiCharacterMovementComponent->Velocity.ToString());

		if (!IsWallNearby()) {
			StopWallRun();
		}

		/*if (AureiCharacterMovementComponent->Safe_bWallRunIsRight) {
			Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorRightVector(), WallNormal)));
		}
		else {
			Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(-GetActorRightVector(), WallNormal)));
		}*/



		//float SpeedModifier = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 90.0f), FVector2D(1.0f, 0.5f), AngleWall);
		//float Speed = FMath::Max(InitialWallRunSpeed * SpeedModifier - GetWorld()->GetDeltaSeconds() * WallRunDeceleration, 0.0f);

		/*if (Speed == 0.0f) {
			StopWallRun();
		}
		else {
			FVector WallRunDirection = FVector(GetVelocity().X, GetVelocity().Y, 0.0f).GetSafeNormal();
			AureiCharacterMovementComponent->Velocity = (WallRunDirection * Speed);
		}*/


	}
}

bool APlayerCharacter::CheckNearbyWall(FHitResult& HitResult) const {
	//Configurar el raycast para detectar colisiones con la etiqueta "Wall"
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.TraceTag = "Wall";

	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * 100.f; //Longitud del raycast

	return GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_EngineTraceChannel1, QueryParams);

}

FCollisionQueryParams APlayerCharacter::GetIgnoreCharacterParams() const {

	FCollisionQueryParams Params;

	TArray<AActor*> CharacterChildren;
	GetAllChildActors(CharacterChildren);
	Params.AddIgnoredActors(CharacterChildren);
	Params.AddIgnoredActor(this);

	return Params;
}