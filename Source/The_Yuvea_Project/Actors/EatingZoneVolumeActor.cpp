#include "EatingZoneVolumeActor.h"
#include "Components/BoxComponent.h"
#include "../Characters/SmallDinosaurCharacter.h"
#include "../Characters/BatCharacter.h"

AEatingZoneVolumeActor::AEatingZoneVolumeActor()
{
    PrimaryActorTick.bCanEverTick = false;

    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    RootComponent = BoxComponent;

    BoxComponent->SetBoxExtent(FVector(200.f, 200.f, 100.f));
    BoxComponent->SetCollisionProfileName(TEXT("Trigger"));
    BoxComponent->SetGenerateOverlapEvents(true);
    BoxComponent->SetHiddenInGame(false);
}

void AEatingZoneVolumeActor::BeginPlay()
{
    Super::BeginPlay();

    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEatingZoneVolumeActor::OnOverlapBegin);
    BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AEatingZoneVolumeActor::OnOverlapEnd);
}

void AEatingZoneVolumeActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (AMainCharacter* Character = Cast<AMainCharacter>(OtherActor))
    {
        Character->SetIsNearFoodZone(true);
    }
}

void AEatingZoneVolumeActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (AMainCharacter* Character = Cast<AMainCharacter>(OtherActor))
    {
        Character->SetIsNearFoodZone(false);
    }
}

