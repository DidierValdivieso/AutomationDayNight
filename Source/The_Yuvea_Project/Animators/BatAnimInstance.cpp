#include "BatAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Characters/BatCharacter.h"

UBatAnimInstance::UBatAnimInstance()
    : Speed(0.f)
    , InputData(FVector2D::ZeroVector)
    , bIsInAir(false)
{

}

void UBatAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    APawn* Owner = TryGetPawnOwner();
    if (!Owner) return;

    Speed = Owner->GetVelocity().Size();

    const ABatCharacter* Bat = Cast<ABatCharacter>(Owner);
    if (!Bat) return;

    InputData = Bat->InputData;
    const bool bCurrentlyInAir = Bat->GetCharacterMovement()->IsFalling();

    bIsInAir = bCurrentlyInAir;
}

