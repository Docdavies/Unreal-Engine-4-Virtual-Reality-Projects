// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GripMotionControllerComponent.h"
#include "MotionControllerComponent.h"
#include "VRGripInterface.h"
#include "Components/StaticMeshComponent.h"
#include "VRInteractibleFunctionLibrary.h"
//#include "VRBPDatatypes.h"
//#include "VRExpansionFunctionLibrary.h"
#include "VRButtonComponent.generated.h"

/**
*
*/

// VR Button Types
UENUM(Blueprintable)
enum class EVRButtonType : uint8
{
	Btn_Press,
	Btn_Toggle_Return,
	Btn_Toggle_Stay
};

/** Delegate for notification when the button state changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FVRButtonStateChangedSignature, bool, ButtonState, AActor *, InteractingActor, UPrimitiveComponent *, InteractingComponent);

/** Delegate for notification when the begins a new interaction. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVRButtonStartedInteractionSignature, AActor *, InteractingActor, UPrimitiveComponent *, InteractingComponent);

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent), ClassGroup = (VRExpansionPlugin))
class VREXPANSIONPLUGIN_API UVRButtonComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UVRButtonComponent(const FObjectInitializer& ObjectInitializer);


	~UVRButtonComponent();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);	

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "VRButtonComponent")
		bool IsButtonInUse();

	// Should be called after the button is moved post begin play
	UFUNCTION(BlueprintCallable, Category = "VRButtonComponent")
		void ResetInitialButtonLocation();

	// Sets the button state outside of interaction, bSnapIntoPosition is for Toggle_Stay mode, it will lerp into the new position if this is false
	UFUNCTION(BlueprintCallable, Category = "VRButtonComponent")
		void SetButtonState(bool bNewButtonState, bool bCallButtonChangedEvent = true, bool bSnapIntoPosition = false);

	// Resets the button to its resting location (mostly for Toggle_Stay)
	UFUNCTION(BlueprintCallable, Category = "VRButtonComponent")
		void SetButtonToRestingPosition(bool bLerpToPosition = false);

	// On the button state changing, keep in mind that InteractingActor can be invalid if manually setting the state
	UPROPERTY(BlueprintAssignable, Category = "VRButtonComponent")
		FVRButtonStateChangedSignature OnButtonStateChanged;

	// On the button state changing, keep in mind that InteractingActor can be invalid if manually setting the state
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Button State Changed"))
		void ReceiveButtonStateChanged(bool bCurButtonState, AActor * LastInteractingActor, UPrimitiveComponent * InteractingComponent);

	// On Button beginning interaction (may spam a bit depending on if overlap is jittering)
	UPROPERTY(BlueprintAssignable, Category = "VRButtonComponent")
		FVRButtonStartedInteractionSignature OnButtonBeginInteraction;

	// On Button ending interaction (may spam a bit depending on if overlap is jittering)
	UPROPERTY(BlueprintAssignable, Category = "VRButtonComponent")
		FVRButtonStartedInteractionSignature OnButtonEndInteraction;

	// On Button beginning interaction (may spam a bit depending on if overlap is jittering)
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Button Started Interaction"))
		void ReceiveButtonBeginInteraction(AActor * InteractingActor, UPrimitiveComponent * InteractingComponent);

	// On Button ending interaction (may spam a bit depending on if overlap is jittering)
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Button Started Interaction"))
		void ReceiveButtonEndInteraction(AActor * LastInteractingActor, UPrimitiveComponent * LastInteractingComponent);

	// On the button state changing, keep in mind that InteractingActor can be invalid if manually setting the state
	UPROPERTY(BlueprintReadOnly, Category = "VRButtonComponent")
		TWeakObjectPtr<UPrimitiveComponent> LocalInteractingComponent;

	UPROPERTY(BlueprintReadOnly, Category = "VRButtonComponent")
		TWeakObjectPtr<AActor> LocalLastInteractingActor;

	UPROPERTY(BlueprintReadOnly, Category = "VRButtonComponent")
		TWeakObjectPtr<UPrimitiveComponent> LocalLastInteractingComponent;

	// Whether the button is enabled or not (can be interacted with)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRButtonComponent")
	bool bIsEnabled;

	// Current state of the button, writable to set initial value
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Replicated, Category = "VRButtonComponent")
	bool bButtonState;

	// #TODO: Offer to autocalulate depress distance / speed based on mesh size?

	// Speed that the button de-presses when no longer interacted with
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRButtonComponent")
	float DepressSpeed;

	// Distance that the button depresses
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRButtonComponent")
	float DepressDistance;

	// Type of button this is
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRButtonComponent")
	EVRButtonType ButtonType;

	// Negative on this axis is the depress direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRButtonComponent")
	EVRInteractibleAxis ButtonAxis;

	// Depth at which the button engages (switches)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRButtonComponent")
	float ButtonEngageDepth;

	// Minimum time before the button can be switched again
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRButtonComponent")
	float MinTimeBetweenEngaging;

	// Skips filtering overlaps on the button and lets you manage it yourself, this is the alternative to overriding IsValidOverlap
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRButtonComponent")
		bool bSkipOverlapFiltering;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRButtonComponent")
	bool IsValidOverlap(UPrimitiveComponent * OverlapComponent);

	// Sets the Last interacting actor variable
	void SetLastInteractingActor();

	virtual FVector GetTargetRelativeLocation();

	// Overrides the default of : true and allows for controlling it like in an actor, should be default of off normally with grippable components
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "VRGripInterface|Replication")
		bool bReplicateMovement;

	virtual void PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker) override;

	// Resetting the initial transform here so that it comes in prior to BeginPlay and save loading.
	virtual void OnRegister() override;

	// Now replicating this so that it works correctly over the network
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_InitialRelativeTransform, Category = "VRButtonComponent")
		FTransform_NetQuantize InitialRelativeTransform;

	UFUNCTION()
		virtual void OnRep_InitialRelativeTransform()
	{
		SetButtonToRestingPosition();
	}

protected:

	// Control variables
	FVector InitialLocation;
	bool bToggledThisTouch;
	FVector InitialComponentLoc;
	float LastToggleTime;

	inline float GetAxisValue(FVector CheckLocation)
	{
		switch (ButtonAxis)
		{
		case EVRInteractibleAxis::Axis_X:
			return CheckLocation.X; break;
		case EVRInteractibleAxis::Axis_Y:
			return CheckLocation.Y; break;
		case EVRInteractibleAxis::Axis_Z:
			return CheckLocation.Z; break;
		default:return 0.0f; break;
		}
	}

	inline FVector SetAxisValue(float SetValue)
	{
		FVector vec = FVector::ZeroVector;

		switch (ButtonAxis)
		{
		case EVRInteractibleAxis::Axis_X:
			vec.X = SetValue; break;
		case EVRInteractibleAxis::Axis_Y:
			vec.Y = SetValue; break;
		case EVRInteractibleAxis::Axis_Z:
			vec.Z = SetValue; break;
		}

		return vec;
	}

};