#include "UnrealEngine.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

class AMultiplayerGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AMultiplayerGameMode()
    {
        // Enable replication
        bReplicates = true;
        
        // Set default port for dedicated server
        DefaultPort = 7777;
    }

    virtual void BeginPlay() override
    {
        Super::BeginPlay();
        
        // Initialize server
        if (GetNetMode() == NM_DedicatedServer)
        {
            UE_LOG(LogTemp, Log, TEXT("Dedicated Server Started"));
        }
    }

    virtual void PostLogin(APlayerController* NewPlayer) override 
    {
        Super::PostLogin(NewPlayer);

        // Handle new player connection
        if (NewPlayer)
        {
            UE_LOG(LogTemp, Log, TEXT("New player connected: %s"), *NewPlayer->GetName());
            OnPlayerConnected(NewPlayer);
        }
    }

    virtual void Logout(AController* Exiting) override
    {
        Super::Logout(Exiting);

        // Handle player disconnection
        if (Exiting)
        {
            UE_LOG(LogTemp, Log, TEXT("Player disconnected: %s"), *Exiting->GetName());
            OnPlayerDisconnected(Exiting);
        }
    }

protected:
    // Called when a new player connects
    virtual void OnPlayerConnected(APlayerController* NewPlayer)
    {
        // Synchronize game state with new player
        SyncGameState(NewPlayer);
    }

    // Called when a player disconnects
    virtual void OnPlayerDisconnected(AController* ExitingPlayer)
    {
        // Clean up player data
        RemovePlayerState(ExitingPlayer);
    }

    // Synchronize game state with client
    void SyncGameState(APlayerController* Player)
    {
        if (Player && Player->PlayerState)
        {
            // Update player position and other relevant game state
            FVector PlayerLocation = Player->GetPawn()->GetActorLocation();
            Player->PlayerState->SetReplicates(true);
            
            // Broadcast updated state to all clients
            MulticastGameState();
        }
    }

    UFUNCTION(NetMulticast, Reliable)
    void MulticastGameState()
    {
        // Broadcast current game state to all connected clients
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            APlayerController* PC = It->Get();
            if (PC && PC->PlayerState)
            {
                // Update client with latest game state
                UpdateClientGameState(PC);
            }
        }
    }

    void UpdateClientGameState(APlayerController* Client)
    {
        if (Client)
        {
            // Send relevant game state data to client
            // This could include player positions, scores, etc.
        }
    }

    void RemovePlayerState(AController* Player)
    {
        if (Player && Player->PlayerState)
        {
            // Clean up player state data
            Player->PlayerState->Destroy();
        }
    }
};
