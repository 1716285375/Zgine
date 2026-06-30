# Acceptance Criteria

1. Editor can enter and exit Play Mode.
2. Runtime systems start and stop through a single lifecycle path.
3. Editing World is not mutated by Play Mode runtime-only changes.
4. Play Mode state changes are observable by Editor UI/events.
5. Stop is safe to call after failed start.
6. Tests verify state transition and world isolation.
