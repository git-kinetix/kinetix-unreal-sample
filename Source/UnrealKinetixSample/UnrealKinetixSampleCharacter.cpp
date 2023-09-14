// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealKinetixSampleCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "KinetixDeveloperSettings.h"
#include "Components/CapsuleComponent.h"

#include "Components/KinetixCharacterComponent.h"
#include "Components/AnimSequenceSamplerComponent.h"
#include "Core/KinetixCoreSubsystem.h"
#include "Core/Account/KinetixAccount.h"
#include "Core/Animation/KinetixAnimation.h"
#include "Core/Metadata/KinetixMetadata.h"
#include "Kismet/KismetSystemLibrary.h"

//////////////////////////////////////////////////////////////////////////
// AUnrealKinetixSampleCharacter

AUnrealKinetixSampleCharacter::AUnrealKinetixSampleCharacter()
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleRoot"));
	SetRootComponent(CapsuleComponent);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	SkeletalMeshSource = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Source"));
	SkeletalMeshSource->SetupAttachment(GetRootComponent());
	SkeletalMeshTarget = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Target"));
	SkeletalMeshTarget->SetupAttachment(SkeletalMeshSource);

	KinetixCharacterComponent = CreateDefaultSubobject<UKinetixCharacterComponent>(TEXT("KinetixCharacterComponent"));
	AnimSequenceSamplerComponent = CreateDefaultSubobject<UAnimSequenceSamplerComponent>(
		TEXT("AnimSequenceSamplerComponent"));
}

void AUnrealKinetixSampleCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	UWorld* CurrentWorld = GetWorld();
	if (!IsValid(CurrentWorld))
	{
		UKismetSystemLibrary::PrintString(this,
		                                  TEXT("[AUnrealKinetixSampleCharacter] BeginPlay: World is null !"),
		                                  true, true, FLinearColor::Red, 10.f);
		return;
	}

	UGameInstance* GameInstance = CurrentWorld->GetGameInstance();
	if (!IsValid(GameInstance))
	{
		UKismetSystemLibrary::PrintString(this,
		                                  TEXT("[AUnrealKinetixSampleCharacter] BeginPlay: GameInstance is null !"),
		                                  true, true, FLinearColor::Red, 10.f);
		return;
	}

	KinetixCore = GameInstance->GetSubsystem<UKinetixCoreSubsystem>();
	if (!IsValid(KinetixCore))
	{
		UKismetSystemLibrary::PrintString(this,
		                                  TEXT("[AUnrealKinetixSampleCharacter] BeginPlay: KinetixCore is null !"),
		                                  true, true, FLinearColor::Red, 10.f);
		return;
	}

	FKinetixCoreInitializedDelegate CoreInitializedDelegate;
	CoreInitializedDelegate.BindDynamic(this, &AUnrealKinetixSampleCharacter::OnCoreInitialized);
	KinetixCore->RegisterOrCallOnInitialized(CoreInitializedDelegate);

	FKinetixCoreConfiguration CoreConfiguration;
	UKinetixDeveloperSettings::GetCoreConfiguration(CoreConfiguration);
	KinetixCore->Setup(CoreConfiguration);
}

void AUnrealKinetixSampleCharacter::OnCoreInitialized()
{
	if (!IsValid(KinetixCore))
	{
		UKismetSystemLibrary::PrintString(this,
		                                  TEXT(
			                                  "[AUnrealKinetixSampleCharacter] OnCoreInitialized: KinetixCore is null !"),
		                                  true, true, FLinearColor::Red, 10.f);
		return;
	}

	KinetixCore->KinetixAccount->OnConnectedAccount.AddDynamic(
		this, &AUnrealKinetixSampleCharacter::OnAccountConnected);

	KinetixCore->KinetixAccount->ConnectAccount(Username);
}

void AUnrealKinetixSampleCharacter::OnAccountConnected()
{
	if (!IsValid(KinetixCore))
	{
		UKismetSystemLibrary::PrintString(this,
		                                  TEXT(
			                                  "[AUnrealKinetixSampleCharacter] OnAccountConnected: KinetixCore is null !"),
		                                  true, true, FLinearColor::Red, 10.f);
		return;
	}

	if (!IsValid(SkeletalMeshSource))
	{
		UKismetSystemLibrary::PrintString(this,
		                                  TEXT(
			                                  "[AUnrealKinetixSampleCharacter] OnAccountConnected: SkeletalMeshSource is null !"),
		                                  true, true, FLinearColor::Red, 10.f);
		return;
	}

	KinetixCore->KinetixAnimation->RegisterLocalPlayerAnimInstance(SkeletalMeshSource->GetAnimInstance());
	KinetixCore->KinetixAccount->OnEmoteAssociated.AddDynamic(this, &AUnrealKinetixSampleCharacter::OnEmoteAssociated);

	FAnimationID AnimationID;
	UKinetixDataBlueprintFunctionLibrary::GetAnimationIDFromString(AnimationIDToRegister, AnimationID);
	KinetixCore->KinetixAccount->AssociateEmoteToUser(AnimationID);
}

void AUnrealKinetixSampleCharacter::OnEmoteAssociated(const FString& Response)
{
	if (!IsValid(KinetixCore))
	{
		UKismetSystemLibrary::PrintString(this,
		                                  TEXT(
			                                  "[AUnrealKinetixSampleCharacter] OnEmoteAssociated: KinetixCore is null !"),
		                                  true, true, FLinearColor::Red, 10.f);
		return;
	}

	FOnMetadatasAvailable OnMetadatasAvailableDelegate;
	OnMetadatasAvailableDelegate.BindDynamic(this, &AUnrealKinetixSampleCharacter::OnMetadatasAvailable);
	KinetixCore->KinetixMetadata->GetUserAnimationMetadatas(OnMetadatasAvailableDelegate);
}

void AUnrealKinetixSampleCharacter::OnMetadatasAvailable(bool bSuccess, const TArray<FAnimationMetadata>& Metadatas)
{
	if (!IsValid(KinetixCore))
	{
		UKismetSystemLibrary::PrintString(this,
		                                  TEXT(
			                                  "[AUnrealKinetixSampleCharacter] OnMetadatasAvailable: KinetixCore is null !"),
		                                  true, true, FLinearColor::Red, 10.f);
		return;
	}

	if (Metadatas.IsEmpty())
	{
		UKismetSystemLibrary::PrintString(this,
		                                  TEXT(
			                                  "[AUnrealKinetixSampleCharacter] OnMetadatasAvailable: Received metadatas are empty !"),
		                                  true, true, FLinearColor::Red, 10.f);
		return;
	}

	FString LockID;
	FOnKinetixLocalAnimationLoadingFinished OnKinetixLocalAnimationLoadingFinishedDelegate;
	OnKinetixLocalAnimationLoadingFinishedDelegate.BindDynamic(this, &AUnrealKinetixSampleCharacter::OnAnimationLoaded);

	AnimationIDToPlay = Metadatas[0].Id;
	KinetixCore->KinetixAnimation->LoadLocalPlayerAnimation(AnimationIDToPlay,
	                                                        LockID,
	                                                        OnKinetixLocalAnimationLoadingFinishedDelegate);
}

void AUnrealKinetixSampleCharacter::OnAnimationLoaded(bool bSuccess)
{
	if (!IsValid(KinetixCore))
	{
		UKismetSystemLibrary::PrintString(this,
		                                  TEXT(
			                                  "[AUnrealKinetixSampleCharacter] OnAnimationLoaded: KinetixCore is null !"),
		                                  true, true, FLinearColor::Red, 10.f);
		return;
	}

	KinetixCore->KinetixAnimation->PlayAnimationOnLocalPlayer(AnimationIDToPlay);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUnrealKinetixSampleCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,
		                                   &AUnrealKinetixSampleCharacter::Look);
	}
}

void AUnrealKinetixSampleCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
