#pragma once

#include "CoreMinimal.h"
#include "CogDefines.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CogSampleCharacterMovementComponent.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UCLASS()
class UCogSampleCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

    class FCogSampleSavedMove : public FSavedMove_Character
    {
    public:

        typedef FSavedMove_Character Super;

        ///@brief Resets all saved variables.
        virtual void Clear() override;

        ///@brief Store input commands in the compressed flags.
        virtual uint8 GetCompressedFlags() const override;

        ///@brief This is used to check whether or not two moves can be combined into one.
        ///Basically you just check to make sure that the saved variables are the same.
        virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

        ///@brief Sets up the move before sending it to the server. 
        virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
        
        ///@brief Sets variables on character movement component before making a predictive correction.
        virtual void PrepMoveFor(class ACharacter* Character) override;

        // Sprint
        uint8 SavedRequestSprint : 1;
    };

    class FCogSampleNetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
    {
    public:
        FCogSampleNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);

        typedef FNetworkPredictionData_Client_Character Super;

        ///@brief Allocates a new copy of our custom saved move
        virtual FSavedMovePtr AllocateNewMove() override;
    };

public:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual float GetMaxSpeed() const override;
    virtual float GetMaxAcceleration() const override;
    virtual FRotator GetDeltaRotation(float DeltaTime) const;
    virtual void UpdateFromCompressedFlags(uint8 Flags) override;
    virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
    virtual bool ClientUpdatePositionAfterServerUpdate() override;

    UFUNCTION(BlueprintCallable)
    void StartSprinting();

    UFUNCTION(BlueprintCallable)
    void StopSprinting();

private:

    bool bIsSprinting = false;

#if USE_COG
    FVector DebugLastBottomLocation = FVector::ZeroVector;
    FVector DebugLastVelocity = FVector::ZeroVector;
    bool DebugIsPositionCorrected = false;
#endif //USE_COG
};
