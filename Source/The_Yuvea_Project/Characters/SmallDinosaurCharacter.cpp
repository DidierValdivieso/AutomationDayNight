#include "SmallDinosaurCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include <Kismet/GameplayStatics.h>
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "../Animators/SmallDinosaurAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "Components/InstancedStaticMeshComponent.h"
#include <Kismet/KismetMathLibrary.h>

ASmallDinosaurCharacter::ASmallDinosaurCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> DinoMesh(TEXT("/Game/StylizedCreaturesBundle/Meshes/SmallDinosaur/SK_SmallDinosaur"));
	if (DinoMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(DinoMesh.Object);
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> DinoAnimBP(TEXT("/Game/Blueprints/ABP_SmallDinosaur"));
	if (DinoAnimBP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(DinoAnimBP.Class);
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

	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.1f;
	GetCharacterMovement()->GravityScale = 2.0f;
}

void ASmallDinosaurCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindKey(EKeys::W, IE_Pressed, this, &ASmallDinosaurCharacter::StartSwimForward);
	PlayerInputComponent->BindKey(EKeys::W, IE_Released, this, &ASmallDinosaurCharacter::StopSwimForward);
	PlayerInputComponent->BindKey(EKeys::S, IE_Pressed, this, &ASmallDinosaurCharacter::StartSwimBackward);
	PlayerInputComponent->BindKey(EKeys::S, IE_Released, this, &ASmallDinosaurCharacter::StopSwimBackward);
	PlayerInputComponent->BindKey(EKeys::D, IE_Pressed, this, &ASmallDinosaurCharacter::StartSwimRight);
	PlayerInputComponent->BindKey(EKeys::D, IE_Released, this, &ASmallDinosaurCharacter::StopSwimRight);
	PlayerInputComponent->BindKey(EKeys::A, IE_Pressed, this, &ASmallDinosaurCharacter::StartSwimLeft);
	PlayerInputComponent->BindKey(EKeys::A, IE_Released, this, &ASmallDinosaurCharacter::StopSwimLeft);
	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &ASmallDinosaurCharacter::HandleJump);
	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &ASmallDinosaurCharacter::SwimUp_Pressed);
	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Released, this, &ASmallDinosaurCharacter::SwimUp_Released);
	PlayerInputComponent->BindKey(EKeys::LeftControl, IE_Pressed, this, &ASmallDinosaurCharacter::SwimDown_Pressed);
	PlayerInputComponent->BindKey(EKeys::LeftControl, IE_Released, this, &ASmallDinosaurCharacter::SwimDown_Released);
	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Released, this, &ASmallDinosaurCharacter::JumpToClimbPoint);
	PlayerInputComponent->BindKey(EKeys::G, IE_Pressed, this, &ASmallDinosaurCharacter::TraceForClimbTarget);

	PlayerInputComponent->BindKey(EKeys::LeftAlt, IE_Pressed, this, &ASmallDinosaurCharacter::DetachFromClimb);
}

void ASmallDinosaurCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Controller) return;

	HandleSwimmingVerticalMovement(DeltaTime);
	HandleSwimmingForwardMovement(DeltaTime);
	HandleSwimmingHorizontalMovement(DeltaTime);
	HandleClimbDebug();
	HandleClimbAttach();
	HandleClimbingLock();
}

void ASmallDinosaurCharacter::HandleSwimmingHorizontalMovement(float DeltaTime)
{
	if (!bIsSwimming)
		return;

	FVector ForwardVector = GetActorForwardVector();
	FVector RightVector = GetActorRightVector();
	FVector NewVelocity = FVector::ZeroVector;

	if (bSwimForwardInput)
	{
		NewVelocity += ForwardVector * SwimForwardSpeed;
	}
	else if (bSwimBackwardInput)
	{
		NewVelocity -= ForwardVector * SwimBackwardSpeed;
	}

	if (bSwimRightInput)
	{
		NewVelocity += RightVector * SwimStrafeSpeed;
	}
	else if (bSwimLeftInput)
	{
		NewVelocity -= RightVector * SwimStrafeSpeed;
	}

	NewVelocity.Z = GetCharacterMovement()->Velocity.Z;

	GetCharacterMovement()->Velocity = NewVelocity;
}

void ASmallDinosaurCharacter::HandleSwimmingStrafeMovement(float DeltaTime)
{
	if (!bIsSwimming)
		return;

	FVector RightVector = GetActorRightVector();
	FVector Velocity = GetCharacterMovement()->Velocity;

	if (bSwimRightInput)
	{
		Velocity += RightVector * SwimStrafeSpeed;
	}
	else if (bSwimLeftInput)
	{
		Velocity -= RightVector * SwimStrafeSpeed;
	}
	else
	{
		Velocity.X = 0.f;
		Velocity.Y = 0.f;
	}

	GetCharacterMovement()->Velocity = Velocity;
}

void ASmallDinosaurCharacter::HandleSwimmingForwardMovement(float DeltaTime)
{
	if (!bIsSwimming)
		return;

	FVector ForwardVector = GetActorForwardVector();
	FVector Velocity = GetCharacterMovement()->Velocity;

	if (bSwimForwardInput)
	{
		Velocity.X = ForwardVector.X * SwimForwardSpeed;
		Velocity.Y = ForwardVector.Y * SwimForwardSpeed;
	}
	else if (bSwimBackwardInput)
	{
		Velocity.X = ForwardVector.X * -SwimBackwardSpeed;
		Velocity.Y = ForwardVector.Y * -SwimBackwardSpeed;
	}
	else
	{
		Velocity.X = 0.f;
		Velocity.Y = 0.f;
	}

	GetCharacterMovement()->Velocity = Velocity;
}

void ASmallDinosaurCharacter::SetIsInWater(bool bNewState)
{
	Super::SetIsInWater(bNewState);

	bIsSwimming = bNewState;

	if (bIsSwimming)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Swimming);
		GetCharacterMovement()->GravityScale = 0.0f;
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		GetCharacterMovement()->GravityScale = 2.0f;
	}
}

void ASmallDinosaurCharacter::HandleClimbDebug()
{
	if (bHasClimbTarget)
	{
		DrawDebugSphere(GetWorld(), TargetClimbPoint, 25.f, 12, FColor::Red, false, -1.f, 0);
	}
}

void ASmallDinosaurCharacter::HandleClimbingLock()
{
	if (bIsClimbing)
	{
		SetActorLocation(ClimbAttachLocation);
	}
}

void ASmallDinosaurCharacter::HandleClimbAttach()
{
	if (bWantsToAttach && !bIsClimbing)
	{
		float Distance = FVector::Dist(GetActorLocation(), DesiredAttachPoint);
		if (Distance < 100.f && GetCharacterMovement()->IsFalling())
		{
			FVector Offset = -LastClimbLaunchDir * 40.f;
			FVector FinalAttachLocation = DesiredAttachPoint + Offset;

			SetActorLocation(FinalAttachLocation);
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->DisableMovement();
			GetCharacterMovement()->GravityScale = 0.f;

			ClimbAttachLocation = FinalAttachLocation;
			bIsClimbing = true;
			bWantsToAttach = false;

			FRotator LookAtRotation = LastClimbLaunchDir.ToOrientationRotator();
			SetActorRotation(LookAtRotation);
		}
	}
}

void ASmallDinosaurCharacter::JumpToClimbPoint()
{
	if (bHasClimbTarget)
	{
		if (bIsClimbing)
		{
			DetachFromClimb();
		}

		bWantsToAttach = true;
		DesiredAttachPoint = TargetClimbPoint;

		LastClimbLaunchDir = (DesiredAttachPoint - GetActorLocation()).GetSafeNormal();
		FVector LaunchDir = LastClimbLaunchDir;

		float Distance = FVector::Dist(DesiredAttachPoint, GetActorLocation());
		float LaunchStrength = FMath::Clamp(Distance * 2.f, 800.f, 2000.f);

		LaunchCharacter(LaunchDir * LaunchStrength + FVector(0, 0, 500.f), true, true);
		bHasClimbTarget = false;
	}
}

void ASmallDinosaurCharacter::TryClimbAttach()
{
	if (!bWantsToAttach) return;

	float Distance = FVector::Dist(GetActorLocation(), TargetClimbPoint);

	if (Distance < 120.f)
	{
		FVector Offset = -LastClimbLaunchDir * 40.f;
		FVector FinalAttachLocation = TargetClimbPoint + Offset;

		SetActorLocation(FinalAttachLocation);

		FRotator FinalRotation = UKismetMathLibrary::MakeRotFromXZ(-ClimbSurfaceNormal, GetActorUpVector());
		SetActorRotation(FinalRotation);

		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->GravityScale = 0.f;

		ClimbAttachLocation = FinalAttachLocation;
		bIsClimbing = true;
		bWantsToAttach = false;
	}
}

void ASmallDinosaurCharacter::DetachFromClimb()
{
	if (bIsClimbing)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterMovement()->GravityScale = 2.f;
		GetCharacterMovement()->SetDefaultMovementMode();

		bIsClimbing = false;
		ClimbTargetActor = nullptr;
	}
}

void ASmallDinosaurCharacter::TraceForClimbTarget()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	int32 ViewportSizeX, ViewportSizeY;
	PC->GetViewportSize(ViewportSizeX, ViewportSizeY);

	float ScreenX = ViewportSizeX / 2.f;
	float ScreenY = ViewportSizeY / 2.f;

	FVector WorldLocation;
	FVector WorldDirection;

	if (PC->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldLocation, WorldDirection))
	{
		FVector Start = GetActorLocation() + FVector(0, 0, 50.f);
		FVector End = Start + WorldDirection * 2000.f;

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
		{
			if (Hit.Component.IsValid() && Hit.Component->IsA<UInstancedStaticMeshComponent>())
			{
				TargetClimbPoint = Hit.Location;
				bHasClimbTarget = true;
			}
		}
		else
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f, 0, 2.0f);
		}
	}
}

void ASmallDinosaurCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASmallDinosaurCharacter::HandleSwimmingVerticalMovement(float DeltaTime)
{
	if (!bIsSwimming)
		return;

	FVector Velocity = GetCharacterMovement()->Velocity;

	if (bSwimUpInput)
	{
		Velocity.Z = SwimUpSpeed;
	}
	else if (bSwimDownInput)
	{
		Velocity.Z = -SwimDownSpeed;
	}
	else
	{
		Velocity.Z = 0.0f;
	}

	GetCharacterMovement()->Velocity = Velocity;
}

void ASmallDinosaurCharacter::SwimUp_Pressed()
{
	bSwimUpInput = true;
}

void ASmallDinosaurCharacter::SwimUp_Released()
{
	bSwimUpInput = false;
}

void ASmallDinosaurCharacter::SwimDown_Pressed()
{
	bSwimDownInput = true;
}

void ASmallDinosaurCharacter::SwimDown_Released()
{
	bSwimDownInput = false;
}

void ASmallDinosaurCharacter::Jump()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		Super::Jump();

		if (USmallDinosaurAnimInstance* DinoAnim = Cast<USmallDinosaurAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			DinoAnim->NotifyJumpPressed();
		}
	}
}

void ASmallDinosaurCharacter::HandleJump()
{
	Jump();
}

void ASmallDinosaurCharacter::StartSwimRight() { bSwimRightInput = true; }
void ASmallDinosaurCharacter::StopSwimRight() { bSwimRightInput = false; }
void ASmallDinosaurCharacter::StartSwimLeft() { bSwimLeftInput = true; }
void ASmallDinosaurCharacter::StopSwimLeft() { bSwimLeftInput = false; }

void ASmallDinosaurCharacter::StartSwimForward() { bSwimForwardInput = true; }
void ASmallDinosaurCharacter::StopSwimForward() { bSwimForwardInput = false; }
void ASmallDinosaurCharacter::StartSwimBackward() { bSwimBackwardInput = true; }
void ASmallDinosaurCharacter::StopSwimBackward() { bSwimBackwardInput = false; }