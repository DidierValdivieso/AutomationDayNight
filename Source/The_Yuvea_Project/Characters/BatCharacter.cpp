#include "BatCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "../Animators/BatAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Animation/AnimInstance.h"
#include "UObject/ConstructorHelpers.h"

ABatCharacter::ABatCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BatMesh(TEXT("/Game/StylizedCreaturesBundle/Meshes/Bat/SK_Bat"));
	if (BatMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(BatMesh.Object);
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> BatAnimBP(TEXT("/Game/Blueprints/Animations/ABP_Bat"));
	if (BatAnimBP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(BatAnimBP.Class);
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");

	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = 600.0f;
	SpringArm->SocketOffset = FVector(0.0f, 0.0f, 150.0f);
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));

	CameraComponent->SetupAttachment(SpringArm);
	CameraComponent->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
}

void ABatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &ABatCharacter::StartFlyUp);
	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Released, this, &ABatCharacter::StopFlyVertical);
	PlayerInputComponent->BindKey(EKeys::LeftControl, IE_Pressed, this, &ABatCharacter::StartFlyDown);
	PlayerInputComponent->BindKey(EKeys::LeftControl, IE_Released, this, &ABatCharacter::StopFlyVertical);
}

void ABatCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleMovement(DeltaTime);

	if (bIsFlying)
	{
		UpdateMinFlyHeight();
	}

	if (bIsFlying && VerticalFlightInput != 0.0f)
	{
		const FVector CurrentLocation = GetActorLocation();
		const float DeltaZ = VerticalFlightSpeed * DeltaTime * VerticalFlightInput;
		const float NewZ = FMath::Clamp(CurrentLocation.Z + DeltaZ, MinFlyHeight, MaxFlyHeight);

		SetActorLocation(FVector(CurrentLocation.X, CurrentLocation.Y, NewZ));
	}

	if (bIsFlying && GetCharacterMovement()->IsMovingOnGround())
	{
		bIsFlying = false;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		GetCharacterMovement()->GravityScale = 2.0f;
	}

}

void ABatCharacter::UpdateMinFlyHeight()
{
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, 10000);

	FCollisionQueryParams TraceParams(FName(TEXT("GroundTrace")), false, this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		TraceParams
	);

	if (bHit)
	{
		MinFlyHeight = HitResult.ImpactPoint.Z + 50.0f;
	}
}

void ABatCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABatCharacter::HandleMovement(float DeltaTime)
{
	if (!MovementInput.IsNearlyZero())
	{
		const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
		AddMovementInput(Forward, MovementInput.X);
		AddMovementInput(Right, MovementInput.Y);

		FRotator TargetRotation = Controller->GetControlRotation();
		TargetRotation.Pitch = 0.f;
		TargetRotation.Roll = 0.f;

		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 12.0f);
		SetActorRotation(NewRotation);
	}
	else if (bIsFlying)
	{
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}
}

void ABatCharacter::StartFlyUp()
{
	bIsFlying = true;
	VerticalFlightInput = 1.0f;
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->GravityScale = 0.0f;
}

void ABatCharacter::StartFlyDown()
{
	bIsFlying = true;
	VerticalFlightInput = -1.0f;
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->GravityScale = 0.0f;
}

void ABatCharacter::StopFlyVertical()
{
	VerticalFlightInput = 0.0f;
}