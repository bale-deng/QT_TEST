#!/usr/bin/env python3
"""
Automated timeout test - simulates the Qt application's timeout behavior
"""
import subprocess
import time
import sys
from datetime import datetime

def test_timeout():
    print("=" * 60)
    print("TIMEOUT MECHANISM TEST")
    print("=" * 60)
    print(f"Start time: {datetime.now().strftime('%H:%M:%S')}")
    print("Expected: Process should be terminated after 30 seconds")
    print("")

    # Start the hanging process
    print("Starting test_timeout.py (will hang for 60 seconds)...")
    start_time = time.time()

    try:
        process = subprocess.Popen(
            [sys.executable, "python/test_timeout.py", "TestCity"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        print(f"Process started with PID: {process.pid}")
        print("Waiting for 30 seconds before terminating...")
        print("")

        # Wait for 30 seconds (simulating Qt's timeout)
        for i in range(30):
            if process.poll() is not None:
                print(f"⚠️  Process ended early at {i} seconds")
                break
            time.sleep(1)
            if (i + 1) % 5 == 0:
                print(f"  ... {i + 1} seconds elapsed")

        elapsed = time.time() - start_time

        # Terminate the process (simulating Qt's terminate())
        if process.poll() is None:
            print("")
            print(f"=== TIMEOUT TRIGGERED at {elapsed:.1f} seconds ===")
            print("Terminating process...")
            process.terminate()

            # Wait up to 2 seconds for graceful termination
            try:
                process.wait(timeout=2)
                print("✅ Process terminated gracefully")
            except subprocess.TimeoutExpired:
                print("Process didn't terminate, killing...")
                process.kill()
                process.wait()
                print("✅ Process killed")

        # Get final timing
        end_time = time.time()
        total_elapsed = end_time - start_time

        # Read any output
        stdout, stderr = process.communicate(timeout=1)

        print("")
        print("=" * 60)
        print("TEST RESULTS")
        print("=" * 60)
        print(f"End time: {datetime.now().strftime('%H:%M:%S')}")
        print(f"Total elapsed: {total_elapsed:.2f} seconds")
        print(f"Process exit code: {process.returncode}")

        if stderr:
            print("")
            print("Process stderr output:")
            print(stderr[:500])  # First 500 chars

        print("")
        if 29.0 <= total_elapsed <= 32.0:
            print("✅ SUCCESS: Timeout mechanism worked correctly!")
            print(f"   Process was terminated at ~30 seconds ({total_elapsed:.1f}s)")
            return True
        elif total_elapsed > 32.0:
            print("❌ FAILURE: Process ran too long!")
            print(f"   Expected ~30s, but took {total_elapsed:.1f}s")
            return False
        else:
            print("⚠️  WARNING: Process ended too quickly")
            print(f"   Expected ~30s, but took {total_elapsed:.1f}s")
            return False

    except Exception as e:
        print(f"❌ ERROR: {e}")
        return False

if __name__ == "__main__":
    success = test_timeout()
    sys.exit(0 if success else 1)
