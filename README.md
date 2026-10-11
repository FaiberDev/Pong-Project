# 🎵 PingBall — Rhythm-Pong Prototype

> **A rhythm game prototype built in Unreal Engine 5 with C++**  
> Pong meets osu! — hit color-coded balls in sync with the beat.

---

## 🎮 Concept

PingBall is a prototype that fuses the classic Pong paddle mechanic with rhythm game timing windows. Colored balls travel from launchers toward the paddle in sync with a music track. The player must match the ball's color *and* hit it at the right moment in the beat to score points and build a combo.

---

## 🏗️ Architecture Overview

The project is built entirely in **Unreal Engine 5 with C++**, using a custom subsystem-based architecture to share rhythm data across all gameplay actors.

```
┌─────────────────────────────────────────────────────────────┐
│                    RhythmStoreSubsystem                     │
│        (UWorldSubsystem — single source of truth)           │
│   CurrentBeat · BPM · TimeSignature · OnBeat (delegate)     │
└──────────────┬──────────────────────────┬───────────────────┘
               │ OnBeat event             │ GetCurrentBeat()
               ▼                          ▼
     ┌──────────────────┐      ┌───────────────────────┐
     │   BallGenerator  │      │     ARythmPaddle       │
     │  (spawns balls   │      │  (player pawn with     │
     │   on each beat,  │      │   color switching &    │
     │   look-ahead: 4) │      │   timing windows)      │
     └────────┬─────────┘      └───────────────────────┘
              │ SpawnActor
              ▼
     ┌──────────────────┐      ┌───────────────────────┐
     │      ABall        │      │     BaseGameMode       │
     │  (lerps Start→   │      │  (scoring, combo,      │
     │   Target by beat) │      │   accuracy tracking)  │
     └──────────────────┘      └───────────────────────┘
              ▲                          ▲
              └────── RegisterHit() ─────┘
```

---

## ✨ Key Features & Systems

### 🎼 Quartz-Synced Music System (`AMusicPlayer`)
- Uses **Unreal's Quartz audio subsystem** to play back music with sample-accurate beat callbacks.
- On each beat, fires an event that updates `RhythmStoreSubsystem`'s `CurrentBeat`, keeping all gameplay actors synchronized to the music without any polling.

### 🗂️ JSON Chart Parser (`URhythmChartParser`)
- A static `BlueprintFunctionLibrary` that reads a custom `.json` chart file (e.g. `kamisama.json`) at runtime.
- Parses `BeatToSpawn` and `ChosenThrower` per note into a `TArray<FBeatMapNote>`, which is handed to the `BallGenerator` at game start.

### 🔮 Beat-Accurate Ball Spawning (`ABallGenerator`)
- Subscribes to `RhythmStoreSubsystem::OnBeat` to receive live beat ticks.
- Uses a **4-beat look-ahead** system: balls are spawned 4 beats in advance and lerp smoothly to the paddle's strike zone, arriving exactly on the target beat.
- Supports multiple configurable lane positions (`Throwers` array, editable in-editor with `MakeEditWidget`).

### 🎯 Color-Matching & Timing Windows (`AARythmPaddle`)
- The player switches the paddle color (Red / Green / Blue) with dedicated key inputs.
- On color press, `AttemptHit()` queries all balls inside the `BoxComponent` strike zone, filters by **matching color index**, and picks the ball with the **smallest beat offset** (closest to perfect timing).
- Implements **5 graded timing windows** inspired by osu!:

| Window | Label | Beat Offset |
|--------|-------|-------------|
| 🌈 Rainbow | **Max** | ≤ 45 ms |
| ✅ Normal | **Perfect** | ≤ 90 ms |
| 🔵 | **Great** | ≤ 130 ms |
| 🟡 | **Good** | ≤ 180 ms |
| ⚪ | **Normal** | ≤ 230 ms |
| ❌ | **Miss** | > 230 ms or wrong color |

- An **anti-mash cooldown** (`FTimerHandle`, 50 ms) prevents spamming the same note.
- Fires a `BlueprintImplementableEvent` (`TriggerHitGlow`) so Blueprints can drive visual feedback per hit quality.

### 📊 Scoring System (`ABaseGameMode`)
- Implements an **osu!-style 1,000,000-point scoring formula**:
  - Half the per-note score is weighted by accuracy (`AccuracyWeight`).
  - The other half is multiplied by a **floating bonus multiplier** that grows on Max hits and decays on imperfect hits.
- Tracks **combo**, **max combo**, and **accuracy percentage** (ratio of accuracy points earned vs. maximum possible).
- Broadcasts `OnScoreUpdated` (a `DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams`) so the UI can react without polling.

### 💥 Ball Juice (`ABall`)
- Balls move via a **beat-interpolated lerp** (from spawn position to paddle position, evaluated against `CurrentBeat`).
- On a hit, `TriggerPerfectHitSquish()` initiates a **scale squish** that snaps to a compressed scale and then springs back, driven by `FMath::VInterpTo` in `Tick`.
- Color is assigned randomly at spawn (Red / Green / Blue) using `UMaterialInstanceDynamic::SetVectorParameterValue`.

---

## 🧠 What I Learned

This project was a deep dive into several intermediate-to-advanced Unreal/C++ concepts:

- **UWorldSubsystem** as a dependency-free, always-available data store — avoiding the fragility of direct actor references across the scene.
- **Unreal's Quartz audio system** for sample-accurate musical synchronization, which is fundamentally different from a timer-based approach.
- **Enhanced Input System** (`UInputMappingContext`, `UInputAction`, `UEnhancedInputComponent`) for clean, data-driven input binding.
- **Dynamic Material Instances** (`UMaterialInstanceDynamic`) for runtime color changes without duplicating materials.
- **Delegate broadcasting** (`DECLARE_DYNAMIC_MULTICAST_DELEGATE`) to decouple the scoring system from the UI layer.
- **Overlap-based hit detection** with `UBoxComponent::GetOverlappingActors` and class filtering.
- **JSON parsing** at runtime with `TSharedPtr<FJsonObject>` to drive data-driven level content.
- **Look-ahead scheduling** — separating *when to spawn* from *when to arrive*, making the ball travel time deterministic regardless of framerate.
- **Blueprint / C++ collaboration**: exposing `BlueprintImplementableEvent` and `BlueprintAssignable` delegates so artists can wire up visuals and UI in Blueprints without touching C++.

---

## 🗂️ Project Structure

```
Source/PingBall/
├── Ball/
│   ├── Ball.h / .cpp            — Rhythm ball actor (lerp movement, squish feedback)
│   └── BallGenerator.h / .cpp  — Beat-driven ball spawner with look-ahead
├── Player/
│   └── ARythmPaddle.h / .cpp   — Player pawn (movement, color switch, timing windows)
├── GameMode/
│   └── BaseGameMode.h / .cpp   — Scoring, combo, and accuracy logic
├── Utilities/
│   └── RhythmChartParser.h/.cpp — Runtime JSON chart loader
├── RhythmStoreSubsystem.h/.cpp  — Global rhythm data bus (WorldSubsystem)
└── ColorBall.h / .cpp           — Early prototype ball (physics-based, pre-rhythm)

MusicPlayer/
└── AMusicPlayer.h / .cpp        — Quartz clock setup and beat broadcasting

Metronome/
└── Metronome.h / .cpp           — Standalone metronome utility actor
```

---

## 🚧 Status

This project is **archived / prototype stage**. The core rhythm engine and scoring systems are fully functional. The following were planned but not implemented:

- Full level design and song library
- Results / grade screen UI
- AI opponent paddle
- Polish pass (animations, SFX, menus)

---

*Built with Unreal Engine 5 · C++ · Quartz Audio · Enhanced Input System*
