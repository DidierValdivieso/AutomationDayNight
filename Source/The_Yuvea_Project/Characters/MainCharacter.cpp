#include "MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Animation/AnimInstance.h"
#include "UObject/ConstructorHelpers.h"
#include <Kismet/GameplayStatics.h>
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include <The_Yuvea_Project/Animators/SmallDinosaurAnimInstance.h>

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	WaterOverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WaterOverlapBox"));
	WaterOverlapBox->SetupAttachment(RootComponent);
	WaterOverlapBox->SetBoxExtent(FVector(50.f, 50.f, 100.f));
	WaterOverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WaterOverlapBox->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	WaterOverlapBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WaterOverlapBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);

		PC->bShowMouseCursor = false;

		PC->bEnableClickEvents = false;
		PC->bEnableMouseOverEvents = false;
		PC->bEnableTouchEvents = false;

		if (UGameViewportClient* Viewport = GetWorld()->GetGameViewport())
		{
			Viewport->SetMouseLockMode(EMouseLockMode::LockAlways);
			Viewport->Viewport->CaptureMouse(true);
			Viewport->Viewport->LockMouseToViewport(true);
		}
	}
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Controller) return;

	HandleMovement(DeltaTime);
	UpdateInputDirection();
	DecreaseHydration(DeltaTime * 1.5f);
	DecreaseEnergy(DeltaTime * 1.2f);
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxisKey(EKeys::MouseX, this, &AMainCharacter::OnMouseX);
	PlayerInputComponent->BindAxisKey(EKeys::MouseY, this, &AMainCharacter::OnMouseY);
	PlayerInputComponent->BindKey(EKeys::W, IE_Pressed, this, &AMainCharacter::MoveForward_Pressed);
	PlayerInputComponent->BindKey(EKeys::W, IE_Released, this, &AMainCharacter::MoveForward_Released);
	PlayerInputComponent->BindKey(EKeys::S, IE_Pressed, this, &AMainCharacter::MoveBackward_Pressed);
	PlayerInputComponent->BindKey(EKeys::S, IE_Released, this, &AMainCharacter::MoveBackward_Released);
	PlayerInputComponent->BindKey(EKeys::D, IE_Pressed, this, &AMainCharacter::MoveRight_Pressed);
	PlayerInputComponent->BindKey(EKeys::D, IE_Released, this, &AMainCharacter::MoveRight_Released);
	PlayerInputComponent->BindKey(EKeys::A, IE_Pressed, this, &AMainCharacter::MoveLeft_Pressed);
	PlayerInputComponent->BindKey(EKeys::A, IE_Released, this, &AMainCharacter::MoveLeft_Released);
	PlayerInputComponent->BindKey(EKeys::E, IE_Released, this, &AMainCharacter::TryDrink);
	PlayerInputComponent->BindKey(EKeys::E, IE_Released, this, &AMainCharacter::Eat);
	PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Pressed, this, &AMainCharacter::StartRunning);
	PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Released, this, &AMainCharacter::StopRunning);
}

void AMainCharacter::HandleMovement(float DeltaTime)
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
}

void AMainCharacter::UpdateInputDirection()
{
	const FRotator CameraRot = Controller->GetControlRotation();
	const FVector Forward = FRotationMatrix(CameraRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(CameraRot).GetUnitAxis(EAxis::Y);

	FVector InputDirection = Forward * MovementInput.X + Right * MovementInput.Y;

	if (!InputDirection.IsNearlyZero())
	{
		InputDirection.Normalize();
	}

	float CurrentSpeed = GetVelocity().Size();
	FVector LocalVelocity = GetActorTransform().InverseTransformVectorNoScale(InputDirection * CurrentSpeed);
	InputData = FVector2D(LocalVelocity.Y, LocalVelocity.X);
}

void AMainCharacter::OnMouseX(float Value)
{
	AddControllerYawInput(Value);
}

void AMainCharacter::OnMouseY(float Value)
{
	AddControllerPitchInput(-Value);
}

void AMainCharacter::StartRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AMainCharacter::StopRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AMainCharacter::TryDrink()
{
	if (!bCanDrink)
		return;

	if (GEngine)
	{
		IncreaseHydration(25.f);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Glup glup... drinking water!"));
	}
}

bool AMainCharacter::IsInWater() const
{
	return bIsInWater;
}

void AMainCharacter::SetIsInWater(bool bNewState)
{
	bIsInWater = bNewState;

	if (bIsInWater)
	{
		bCanDrink = true;
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, TEXT("Press E to drink water!"));
		}
	}
	else
	{
		bCanDrink = false;
	}
}

void AMainCharacter::SetIsNearFoodZone(bool bNewState)
{
	bIsNearFoodZone = bNewState;

	if (bIsNearFoodZone)
	{
		if (GEngine && !bHasShownEatPrompt)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Press E to eat food!"));
		}
		bHasShownEatPrompt = true;
	}
	else
	{
		bHasShownEatPrompt = false;
	}
}

float AMainCharacter::GetHydrationRatio() const
{
	return FMath::Clamp(Hydration / 100.f, 0.f, 1.f);
}

void AMainCharacter::DecreaseHydration(float Amount)
{
	Hydration = FMath::Clamp(Hydration - Amount, 0.f, 100.f);

	if (Hydration <= 0.f)
	{
		GetCharacterMovement()->DisableMovement();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("You are dehydrated and can't move!"));
		}
		else if (Hydration <= 20.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("Very low hydration, look for water!"));
		}
	}
}

void AMainCharacter::IncreaseHydration(float Amount)
{
	const bool bWasZero = Hydration <= 0.f;
	Hydration = FMath::Clamp(Hydration + Amount, 0.f, 100.f);

	if (bWasZero && Hydration > 0.f)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Swimming);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Hydrated, you can move again!"));
		}
	}
}

void AMainCharacter::Eat()
{
	if (!bIsNearFoodZone)
		return;

	bool bWasZero = Energy <= 0.f;
	Energy = FMath::Clamp(Energy + 25.f, 0.f, 100.f);

	if (bWasZero && Energy > 0.f)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("You've regained energy. You can move again!"));
		}
	}
}

float AMainCharacter::GetEnergyRatio() const
{
	return FMath::Clamp(Energy / 100.f, 0.f, 1.f);
}

void AMainCharacter::DecreaseEnergy(float Amount)
{
	Energy = FMath::Clamp(Energy - Amount, 0.f, 100.f);

	if (Energy <= 0.f)
	{
		GetCharacterMovement()->DisableMovement();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Without energy, you can't move!"));
		}
	}
}

void AMainCharacter::MoveForward_Pressed() { MovementInput.X += 1.0f; }
void AMainCharacter::MoveForward_Released() { MovementInput.X -= 1.0f; }

void AMainCharacter::MoveBackward_Pressed() { MovementInput.X -= 1.0f; }
void AMainCharacter::MoveBackward_Released() { MovementInput.X += 1.0f; }

void AMainCharacter::MoveRight_Pressed() { MovementInput.Y += 1.0f; }
void AMainCharacter::MoveRight_Released() { MovementInput.Y -= 1.0f; }

void AMainCharacter::MoveLeft_Pressed() { MovementInput.Y -= 1.0f; }
void AMainCharacter::MoveLeft_Released() { MovementInput.Y += 1.0f; }