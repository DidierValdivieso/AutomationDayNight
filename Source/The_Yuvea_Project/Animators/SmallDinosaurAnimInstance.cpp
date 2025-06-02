#include "SmallDinosaurAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Characters/SmallDinosaurCharacter.h"

USmallDinosaurAnimInstance::USmallDinosaurAnimInstance()
    : Speed(0.f)
    , InputData(FVector2D::ZeroVector)
    , bIsInAir(false)
    , bIsJumpingStart(false)
    , bIsJumpingEnd(false)
    , JumpStartTimer(0.f)
    , bJumpStartTimerActive(false)
{

}

void USmallDinosaurAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    APawn* Owner = TryGetPawnOwner();
    if (!Owner) return;

    Speed = Owner->GetVelocity().Size();

    const ASmallDinosaurCharacter* Dino = Cast<ASmallDinosaurCharacter>(Owner);
    if (!Dino) return;

    bIsInWater = Dino->IsInWater();
    bIsSwimming = Dino->GetCharacterMovement()->IsSwimming();
    InputData = Dino->InputData;
    const bool bCurrentlyInAir = Dino->GetCharacterMovement()->IsFalling();

    if (!bIsInAir && bCurrentlyInAir)
    {
        bIsJumpingStart = true;
        bIsJumpingEnd = false;
        bJumpStartTimerActive = true;
        JumpStartTimer = 0.f;
    }

    if (bJumpStartTimerActive)
    {
        JumpStartTimer += DeltaSeconds;
        if (JumpStartTimer >= 0.3f)
        {
            bIsJumpingStart = false;
            bIsJumpingEnd = bCurrentlyInAir;
            bJumpStartTimerActive = false;
        }
    }

    if (!bCurrentlyInAir)
    {
        bIsJumpingStart = false;
        bIsJumpingEnd = false;
        bJumpStartTimerActive = false;
        JumpStartTimer = 0.f;
    }

    bIsInAir = bCurrentlyInAir;
}

void USmallDinosaurAnimInstance::NotifyJumpPressed()
{
    bIsJumpingStart = true;
    bIsJumpingEnd = false;
    bJumpStartTimerActive = true;
    JumpStartTimer = 0.f;
}
