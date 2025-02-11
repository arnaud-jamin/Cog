#include "CogSampleCharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CogSampleAttributeSet_Speed.h"
#include "CogSampleDefines.h"
#include "CogSampleFunctionLibrary_Tag.h"
#include "CogSampleLogCategories.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

#if ENABLE_COG
#include "CogDebugDraw.h"
#include "CogDebug.h"
#endif //ENABLE_COG

//--------------------------------------------------------------------------------------------------------------------------
// UCogSampleCharacterMovementComponent::FCogSampleSavedMove
//--------------------------------------------------------------------------------------------------------------------------
uint8 UCogSampleCharacterMovementComponent::FCogSampleSavedMove::GetCompressedFlags() const
{
    uint8 Result = Super::GetCompressedFlags();

    if (SavedRequestSprint)
    {
        Result |= FLAG_Custom_0;
    }

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleCharacterMovementComponent::FCogSampleSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
    //----------------------------------------------------------------------------------------------
    // Set which moves can be combined together. This will depend on the bit flags that are used.
    //----------------------------------------------------------------------------------------------

    if (SavedRequestSprint != ((FCogSampleSavedMove*)&NewMove)->SavedRequestSprint)
    {
        return false;
    }


    return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleCharacterMovementComponent::FCogSampleSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
    Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

    if (UCogSampleCharacterMovementComponent* CharacterMovement = Cast<UCogSampleCharacterMovementComponent>(Character->GetCharacterMovement()))
    {
        SavedRequestSprint = CharacterMovement->bIsSprinting;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleCharacterMovementComponent::FCogSampleSavedMove::PrepMoveFor(ACharacter* Character)
{
    Super::PrepMoveFor(Character);
}

//--------------------------------------------------------------------------------------------------------------------------
// UCogSampleCharacterMovementComponent::FCogSampleNetworkPredictionData_Client
//--------------------------------------------------------------------------------------------------------------------------

UCogSampleCharacterMovementComponent::FCogSampleNetworkPredictionData_Client::FCogSampleNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement)
    : Super(ClientMovement)
{
}

//--------------------------------------------------------------------------------------------------------------------------
FSavedMovePtr UCogSampleCharacterMovementComponent::FCogSampleNetworkPredictionData_Client::AllocateNewMove()
{
    return FSavedMovePtr(new FCogSampleSavedMove());
}

//--------------------------------------------------------------------------------------------------------------------------
// UCogSampleCharacterMovementComponent
//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
    Super::UpdateFromCompressedFlags(Flags);

    //-------------------------------------------------------------------------------------------------------------------
    // The Flags parameter contains the compressed input flags that are stored in the saved move.
    // UpdateFromCompressed flags simply copies the flags from the saved move into the movement component.
    // It basically just resets the movement component to the state when the move was made so it can simulate from there.
    //-------------------------------------------------------------------------------------------------------------------

    //-----------------------
    // Sprint
    //-----------------------
    bIsSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

//--------------------------------------------------------------------------------------------------------------------------
FNetworkPredictionData_Client* UCogSampleCharacterMovementComponent::GetPredictionData_Client() const
{
    check(PawnOwner != NULL);

    if (!ClientPredictionData)
    {
        UCogSampleCharacterMovementComponent* MutableThis = const_cast<UCogSampleCharacterMovementComponent*>(this);
        MutableThis->ClientPredictionData = new FCogSampleNetworkPredictionData_Client(*this);
    }

    return ClientPredictionData;
}

//--------------------------------------------------------------------------------------------------------------------------
float UCogSampleCharacterMovementComponent::GetMaxSpeed() const
{
    UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawnOwner(), true);

    if (AbilitySystemComponent == nullptr)
    {
        return Super::GetMaxSpeed();
    }

    if (AbilitySystemComponent->HasMatchingGameplayTag(Tag_Status_Immobilized))
    {
        return 0.0f;
    }

    if (AbilitySystemComponent->HasMatchingGameplayTag(Tag_Status_Stunned))
    {
        return 0.0f;
    }

    if (AbilitySystemComponent->HasMatchingGameplayTag(Tag_Status_Dead))
    {
        return 0.0f;
    }

    if (AbilitySystemComponent->HasMatchingGameplayTag(Tag_Status_Revived))
    {
        return 0.0f;
    }

    const float Speed = AbilitySystemComponent->GetNumericAttribute(UCogSampleAttributeSet_Speed::GetSpeedAttribute());
    return Speed;
}

//--------------------------------------------------------------------------------------------------------------------------
float UCogSampleCharacterMovementComponent::GetMaxAcceleration() const
{
    UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawnOwner(), true);

    if (AbilitySystemComponent == nullptr)
    {
        return Super::GetMaxAcceleration();
    }

    const float MaxAccel = AbilitySystemComponent->GetNumericAttribute(UCogSampleAttributeSet_Speed::GetMaxAccelerationAttribute());
    return MaxAccel;
}

//--------------------------------------------------------------------------------------------------------------------------
FRotator UCogSampleCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
    UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawnOwner(), true);
    if (AbilitySystemComponent == nullptr)
    {
        return Super::GetDeltaRotation(DeltaTime);
    }

    if (AbilitySystemComponent->HasMatchingGameplayTag(Tag_Status_Stunned))
    {
        return FRotator::ZeroRotator;
    }

    if (AbilitySystemComponent->HasMatchingGameplayTag(Tag_Status_Dead))
    {
        return FRotator::ZeroRotator;
    }

    if (AbilitySystemComponent->HasMatchingGameplayTag(Tag_Status_Revived))
    {
        return FRotator::ZeroRotator;
    }

    return Super::GetDeltaRotation(DeltaTime);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleCharacterMovementComponent::StartSprinting()
{
    bIsSprinting = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleCharacterMovementComponent::StopSprinting()
{
    bIsSprinting = false;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleCharacterMovementComponent::FCogSampleSavedMove::Clear()
{
    Super::Clear();
    SavedRequestSprint = false;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleCharacterMovementComponent::BeginPlay()
{
    Super::BeginPlay();

#if ENABLE_COG
    const ACharacter* Character = GetCharacterOwner();
    const UCapsuleComponent* CapsuleComponent = Character->GetCapsuleComponent();
    DebugLastBottomLocation = Character->GetActorLocation() - FVector::UpVector * CapsuleComponent->GetScaledCapsuleHalfHeight();
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
#if ENABLE_COG

    const ACharacter* Character = GetCharacterOwner();
    const UCapsuleComponent* CapsuleComponent = Character->GetCapsuleComponent();

    if (FCogDebug::IsDebugActiveForObject(GetPawnOwner()))
    {
        FCogDebug::Plot(Character, "Move Input X", GetPendingInputVector().X);
        FCogDebug::Plot(Character, "Move Input Y", GetPendingInputVector().Y);
        FCogDebug::Plot(Character, "Move Input Local X", FVector::DotProduct(GetPawnOwner()->GetActorRightVector(), GetPendingInputVector()));
        FCogDebug::Plot(Character, "Move Input Local Y", FVector::DotProduct(GetPawnOwner()->GetActorForwardVector(), GetPendingInputVector()));
    }

#endif //ENABLE_COG

    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if ENABLE_COG
       
    const FVector DebugLocation = Character->GetActorLocation();
    const FVector DebugBottomLocation = DebugLocation - FVector::UpVector * CapsuleComponent->GetScaledCapsuleHalfHeight();

    if (FCogDebug::IsDebugActiveForObject(GetPawnOwner()))
    {
        const FRotator Rotation = Character->GetActorRotation();
        const FVector Forward = Character->GetActorForwardVector();
        const FVector LocalVelocity = Rotation.UnrotateVector(Velocity);
        const FVector LocalAcceleration = Rotation.UnrotateVector(GetCurrentAcceleration());
        const FVector VelocityDelta = (Velocity - DebugLastVelocity) / DeltaTime;
        const FVector LocalVelocityDelta = Rotation.UnrotateVector(VelocityDelta);

        FCogDebug::Plot(Character, "Location X", DebugBottomLocation.X);
        FCogDebug::Plot(Character, "Location Y", DebugBottomLocation.Y);
        FCogDebug::Plot(Character, "Location Z", DebugBottomLocation.Z);
        FCogDebug::Plot(Character, "Rotation Yaw", Character->GetActorRotation().Yaw);
        FCogDebug::Plot(Character, "Acceleration X", GetCurrentAcceleration().X);
        FCogDebug::Plot(Character, "Acceleration Y", GetCurrentAcceleration().Y);
        FCogDebug::Plot(Character, "Acceleration Z", GetCurrentAcceleration().Z);
        FCogDebug::Plot(Character, "Acceleration Local X", LocalAcceleration.X);
        FCogDebug::Plot(Character, "Acceleration Local Y", LocalAcceleration.Y);
        FCogDebug::Plot(Character, "Acceleration Local Z", LocalAcceleration.Z);
        FCogDebug::Plot(Character, "Velocity X", Velocity.X);
        FCogDebug::Plot(Character, "Velocity Y", Velocity.Y);
        FCogDebug::Plot(Character, "Velocity Z", Velocity.Z);
        FCogDebug::Plot(Character, "Velocity Local X", LocalVelocity.X);
        FCogDebug::Plot(Character, "Velocity Local Y", LocalVelocity.Y);
        FCogDebug::Plot(Character, "Velocity Local Z", LocalVelocity.Z);
        FCogDebug::Plot(Character, "Velocity Delta X", VelocityDelta.X);
        FCogDebug::Plot(Character, "Velocity Delta Y", VelocityDelta.Y);
        FCogDebug::Plot(Character, "Velocity Delta Z", VelocityDelta.Z);
        FCogDebug::Plot(Character, "Velocity Delta Local X", LocalVelocityDelta.X);
        FCogDebug::Plot(Character, "Velocity Delta Local Y", LocalVelocityDelta.Y);
        FCogDebug::Plot(Character, "Velocity Delta Local Z", LocalVelocityDelta.Z);
        FCogDebug::Plot(Character, "Speed", Velocity.Length());

        const FVector Delta = DebugBottomLocation - DebugLastBottomLocation;
        const FColor Color = DebugIsPositionCorrected ? FColor::Blue : FColor::Yellow;
        if (Delta.IsNearlyZero() == false)
        {
            FCogDebugDraw::Arrow(LogCogPosition, this, DebugLastBottomLocation, DebugBottomLocation, Color, true, 0);
        }

        const FColor CapsuleColor = CapsuleComponent->GetCollisionEnabled() == ECollisionEnabled::NoCollision ? FColor::Black : FColor::White;
        FCogDebugDraw::Capsule(LogCogCollision, Character, CapsuleComponent->GetComponentLocation(), CapsuleComponent->GetScaledCapsuleHalfHeight(), CapsuleComponent->GetScaledCapsuleRadius(), CapsuleComponent->GetComponentQuat(), CapsuleColor, false, 0);
        FCogDebugDraw::Axis(LogCogCollision, Character, CapsuleComponent->GetComponentLocation(), CapsuleComponent->GetComponentRotation(), 50.0f, false, 0);
        FCogDebugDraw::Arrow(LogCogRotation, Character, DebugLocation, DebugLocation + Rotation.Vector() * 100.0f, FColor::Green, true, 0);
        FCogDebugDraw::Arrow(LogCogControlRotation, Character, DebugLocation, DebugLocation + Character->GetControlRotation().Vector() * 100.f, FColor::Silver, true, 0);
        FCogDebugDraw::Arrow(LogCogBaseAimRotation, Character, DebugLocation, DebugLocation + Character->GetBaseAimRotation().Vector() * 100.f, FColor::Red, true, 0);
        FCogDebugDraw::Skeleton(LogCogSkeleton, Character->GetMesh(), FColor::White, false, 1);
    }
    
    DebugLastBottomLocation = DebugBottomLocation;
    DebugLastVelocity = Velocity;
    DebugIsPositionCorrected = false;

#endif //ENABLE_COG

}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleCharacterMovementComponent::ClientUpdatePositionAfterServerUpdate()
{
    bool Result = Super::ClientUpdatePositionAfterServerUpdate();

#if ENABLE_COG
    DebugIsPositionCorrected = true;
#endif //ENABLE_COG

    return Result;
}


//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleCharacterMovementComponent::PossessedBy(AController* NewController)
{
    if (NewController != nullptr && NewController->IsPlayerController())
    {
        bWasAvoidanceEnabled = bUseRVOAvoidance;
        SetAvoidanceEnabled(false);
    }
    else
    {
        SetAvoidanceEnabled(bWasAvoidanceEnabled);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleCharacterMovementComponent::UnPossessed()
{
    //---------------------------------------------------------------------------------------
    // Make sure the character doesn't keep his velocity while not controlled anymore.
    //---------------------------------------------------------------------------------------
    ConsumeInputVector();
}