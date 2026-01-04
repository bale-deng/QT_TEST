# Deployment Optimization - Final Plan (V3)

Status: Finalized
Scope: File I/O safety, encoding fidelity, zoom/render performance, save behavior

## 1) File Safety (Open)
- Block or warn on non-Markdown file types; allow override only with explicit confirmation.
- Enforce a size threshold before read (e.g., 10MB hard limit, 10-100MB warn+open without preview).
- For large files, open in editor-only mode (disable preview) to prevent UI freeze.
- Always show a clear error when open fails (path + error string).

Acceptance:
- Opening a 100MB file never hangs the UI; user gets a clear warning or a safe open mode.
- Non-Markdown files are rejected or opened only with explicit confirmation.

## 2) Encoding & Path Robustness (Read/Write)
- Detect BOM on read (UTF-8/UTF-16/UTF-32). Store detected encoding per file.
- If no BOM, attempt UTF-8; on decode errors, prompt for encoding selection.
- Save using original encoding when known; default to UTF-8 otherwise.
- Use QSaveFile for atomic writes to avoid partial/garbled output on failure.

Acceptance:
- Chinese/Japanese paths save and reopen without mojibake.
- Non-UTF-8 content preserves round-trip after open+save.

## 3) UI Performance (Zoom + Preview)
- Debounce preview re-render (e.g., 150-250ms idle timer).
- During rapid zoom, update zoom only; delay preview regeneration until input settles.
- Cap preview refresh rate (e.g., <= 5 updates/sec).

Acceptance:
- Rapid Ctrl+wheel zoom does not tear or stutter; preview updates smoothly after input stops.

## 4) Save Behavior (New vs Existing)
- Save As: set current file path only after a successful write.
- Save: if no path, route to Save As; if write fails, keep modified flag true.
- Optional: confirm overwrite when Save As targets an existing file.

Acceptance:
- Save As failure never changes current file path.
- Save for new files always shows Save As dialog.

## Validation Checklist
- Open 100MB file: app stays responsive.
- Open non-md file: blocked or confirmed.
- Save file with non-ASCII path: readable after reopen.
- Fast zoom test (10+ scrolls/sec): no visible tearing.
- Save As failure: path and modified state remain correct.
